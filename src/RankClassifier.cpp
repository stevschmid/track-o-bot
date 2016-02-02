#include "RankClassifier.h"
#include "MLP.h"

#include "Hearthstone.h"

#include <cassert>
#include <cmath>

#define RC_PROBA_THRESHOLD 0.95 // return 0/RANK_UNKNOWN when best output below this threshold (prob. since we use SOFTMAX)

// The width / height of the rank label
#define RC_LABEL_WIDTH 28
#define RC_LABEL_HEIGHT 28

// The position where the labels can be obtained from
#define RC_CAPTURE_X 32
#define RC_CAPTURE_Y 960
#define RC_CAPTURE_WIDTH 40
#define RC_CAPTURE_HEIGHT 40

#define RC_CAPTURE_SCREEN_WIDTH  1920
#define RC_CAPTURE_SCREEN_HEIGHT 1080

// Ranks are displayed in white
// Binarize images to get rid of the noise
// Use HSV thresholds (matching pixel values are considered 1, otherwise 0)
#define RC_BINARIZE_MAX_SATURATION 5
#define RC_BINARIZE_MIN_VALUE 50

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

RankClassifier::RankClassifier() {
  LoadMLP();
}

MLP::Vector JsonArrayToVector( const QJsonValue& val ) {
  MLP::Vector res;
  const QJsonArray& arr = val.toArray();
  std::transform( arr.begin(), arr.end(), std::back_inserter( res ), []( QJsonValue ref ) {
    return ( float )ref.toDouble();
  });
  return res;
}

void RankClassifier::LoadMLP() {
  QFile file( ":/data/rank_classifier.json" );
  bool opened = file.open( QIODevice::ReadOnly | QIODevice::Text );
  assert( opened );

  QByteArray jsonData = file.readAll();
  QJsonParseError error;
  QJsonObject jsonLayers = QJsonDocument::fromJson( jsonData, &error ).object();
  assert( error.error == QJsonParseError::NoError );

  for( QJsonValueRef jsonLayerRef : jsonLayers ) {
    QJsonObject jsonLayer = jsonLayerRef.toObject();
    MLP::Layer layer;

    // type
    QString type = jsonLayer["type"].toString();
    if( type == "SOFTMAX" ) {
      layer.type = MLP::LAYER_SOFTMAX;
    } else if( type == "SIGMOID" ) {
      layer.type = MLP::LAYER_SIGMOID;
    } else if( type == "RECTIFIER" ) {
      layer.type = MLP::LAYER_RECTIFIER;
    } else {
      assert( "Unknown layer type" );
    }

    // biases
    layer.biases = JsonArrayToVector( jsonLayer[ "biases" ] );

    // weight matrix
    QJsonArray rows = jsonLayer[ "weights" ].toArray();
    std::transform( rows.begin(), rows.end(), std::back_inserter( layer.weights ), []( QJsonValueRef ref ) {
      return JsonArrayToVector( ref );
    });

    DBG( "Load layer: %d biases | %dx%d weights (type %d)",
        layer.biases.size(),
        layer.weights.size(),
        layer.weights.front().size(),
        layer.type);

    mMLP.AddLayer( layer );
  }
}

int RankClassifier::Classify( const QImage& label, float *outScore ) const {
  assert( label.width() == RC_LABEL_WIDTH );
  assert( label.height() == RC_LABEL_HEIGHT );

  MLP::Vector input = BinarizeImageSV( label, RC_BINARIZE_MAX_SATURATION, RC_BINARIZE_MIN_VALUE );

  MLP::Vector result = mMLP.Compute( input );
  std::vector< std::pair<int, float> > scores;
  for( int i = 0; i < (int)result.size(); i++ ) {
    scores.push_back( std::pair< int, float >( i, result[ i ] ) );
  }

  std::sort( scores.begin(), scores.end(), []( const std::pair< int, float>& p1, const std::pair< int, float>& p2 ) {
    return p1.second > p2.second;
  });

  for( int i = 0; i < (int)scores.size(); i++ ) {
    DBG( "Rank %d = %f", scores[i].first + 1, scores[i].second );
  }

  float bestScore = scores.front().second;
  if( outScore )
    *outScore = bestScore;

  int rank = 0; // RANK_UNKNOWN
  if( bestScore >= RC_PROBA_THRESHOLD ) {
    rank = scores.front().first + 1;
  }

  DBG( "Best score %.3f >= %.3f. Return %d", bestScore, RC_PROBA_THRESHOLD, rank );
  return rank;
}

int RankClassifier::DetectCurrentRank( float *outScore, QImage *outLabel ) {
  // Grab label
  QImage raw = Hearthstone::Instance()->Capture( RC_CAPTURE_SCREEN_WIDTH, RC_CAPTURE_SCREEN_HEIGHT,
      RC_CAPTURE_X, RC_CAPTURE_Y,
      RC_CAPTURE_WIDTH, RC_CAPTURE_HEIGHT ).toImage();

  QImage label = raw.scaled( QSize( RC_LABEL_WIDTH, RC_LABEL_HEIGHT ),
    Qt::IgnoreAspectRatio,
    Qt::SmoothTransformation );

  if( label.width() != RC_LABEL_WIDTH || label.height() != RC_LABEL_HEIGHT ) {
    return 0;
  }

  if( outLabel )
    *outLabel = label;

  return Classify( label, outScore );
}

MLP::Vector RankClassifier::BinarizeImageSV( const QImage& img, float maxSaturation, float minValue ) {
  MLP::Vector bin( img.width() * img.height() );

  float sumX = 0.0f;
  float sumY = 0.0f;
  int area = 0;

  for( int y = 0; y < img.height(); y++ ) {
    for( int x = 0; x < img.width(); x++ ) {
      int idx = y * img.width() + x;

      QRgb pixel = img.pixel( x, y );

      int h, s, v;
      QColor( pixel ).getHsv( &h, &s, &v );

      h = std::max(0, h);
      s = int(float(s) / 255.0f * 100.0f);
      v = int(float(v) / 255.0f * 100.0f);

      bool white = ( s <= maxSaturation && v >= minValue );
      bin[ idx ] = white ? 1.0f : 0.0f;

      if( white ) {
        sumX += (float(x) + 0.5f);
        sumY += (float(y) + 0.5f);
        area++;
      }
    }
  }

  MLP::Vector binCentered( img.width() * img.height(), 0.0f );

  float imageCenterX = img.width() * 0.5f;
  float imageCenterY = img.height() * 0.5f;

  float gravityX = area ? sumX / area : imageCenterX;
  float gravityY = area ? sumY / area : imageCenterY;

  int offsetX = std::round(imageCenterX - gravityX);
  int offsetY = std::round(imageCenterY - gravityY);

  for( int y = 0; y < img.height(); y++ ) {
    for( int x = 0; x < img.width(); x++ ) {
      int idx = y * img.width() + x;
      float val = bin[ idx ];

      int newX = x + offsetX;
      int newY = y + offsetY;

      if( newX >= 0 && newX < img.width() &&
          newY >= 0 && newY < img.height() )
      {
        int newIdx = newY * img.width() + newX;
        binCentered[ newIdx ] = val;
      }
    }
  }

  return binCentered;
}

