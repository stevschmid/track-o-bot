#include "RankClassifier.h"
#include "MLP.h"

#include "Hearthstone.h"

#include <cassert>

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
    layer.type = MLP::LAYER_SIGMOID;
    if( jsonLayer["type"].toString() == "SOFTMAX" ) {
      layer.type = MLP::LAYER_SOFTMAX;
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

int RankClassifier::Classify( const QImage& label ) const {
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

  return scores.front().first + 1;
}

int RankClassifier::DetectCurrentRank() const {
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

  return Classify( label );
}

MLP::Vector RankClassifier::BinarizeImageSV( const QImage& img, float maxSaturation, float minValue ) {
  MLP::Vector out( img.width() * img.height() );

  for( int y = 0; y < img.height(); y++ ) {
    for( int x = 0; x < img.width(); x++ ) {
      int idx = y * img.width() + x;

      QRgb pixel = img.pixel( x, y );

      int h, s, v;
      QColor( pixel ).getHsv( &h, &s, &v );
      bool white = ( s <= maxSaturation && v >= minValue );
      out[ idx ] = white ? 1.0f : 0.0f;
    }
  }

  return out;
}

