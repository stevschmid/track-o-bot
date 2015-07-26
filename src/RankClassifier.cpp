#include "RankClassifier.h"
#include "NeuralNetwork.h"

#include "RankClassifierData.h"

#include "Hearthstone.h"

#include <cassert>

#define RC_LABEL_WIDTH 40
#define RC_LABEL_HEIGHT 40

#define RC_CAPTURE_X 32
#define RC_CAPTURE_Y 960

#define RC_CAPTURE_SCREEN_WIDTH  1920
#define RC_CAPTURE_SCREEN_HEIGHT 1080

#define RC_BINARIZE_THRESHOLD 0.7f

RankClassifier::RankClassifier()
  : mNeuralNetwork( BuildNN() )
{
  assert( RC_LABEL_WIDTH * RC_LABEL_HEIGHT == NN_INPUT_NODES );
}

NN::NeuralNetwork RankClassifier::BuildNN() const {
  NN::Vector hiddenLayerBias( NN_HIDDEN_BIAS, NN_HIDDEN_BIAS + NN_HIDDEN_NODES );
  NN::Vector outputLayerBias( NN_OUTPUT_BIAS, NN_OUTPUT_BIAS + NN_OUTPUT_NODES );

  NN::Matrix hiddenLayerWeights;
  NN::Matrix outputLayerWeights;

  for( int i = 0; i < NN_HIDDEN_NODES; i++ ) {
    const float *w = NN_HIDDEN_WEIGHTS[ i ];
    hiddenLayerWeights.push_back( NN::Vector( w, w + NN_INPUT_NODES ) );
  }

  for( int i = 0; i < NN_OUTPUT_NODES; i++ ) {
    const float *w = NN_OUTPUT_WEIGHTS[ i ];
    outputLayerWeights.push_back( NN::Vector( w, w + NN_HIDDEN_NODES ) );
  }

  return NN::NeuralNetwork( hiddenLayerBias, hiddenLayerWeights,
      outputLayerBias, outputLayerWeights );
}


bool compareScore( const std::pair< int, float >& p1, const std::pair< int, float >& p2 ) {
  return p1.second > p2.second;
}

int RankClassifier::Classify( const QImage& label ) const {
  NN::Vector input( RC_LABEL_WIDTH * RC_LABEL_HEIGHT );

  assert( label.width() == RC_LABEL_WIDTH );
  assert( label.height() == RC_LABEL_HEIGHT );

  for( int y = 0; y < RC_LABEL_HEIGHT; y++ ) {
    for( int x = 0; x < RC_LABEL_WIDTH; x++ ) {
      int idx = y * RC_LABEL_WIDTH + x;
      QRgb pixel = label.pixel( x, y );
      float avg = float(qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / (3.0f * 255);

      if( avg >= RC_BINARIZE_THRESHOLD ) {
        input[ idx ] = 1.0f;
      } else {
        input[ idx ] = 0.0f;
      }
    }
  }

  NN::Vector result = mNeuralNetwork.Run( input );
  std::vector< std::pair<int, float> > scores;
  for( int i = 0; i < (int)result.size(); i++ ) {
    scores.push_back( std::pair< int, float >( i, result[ i ] ) );
  }

  std::sort( scores.begin(), scores.end(), compareScore );
  for( int i = 0; i < (int)scores.size(); i++ ) {
    DEBUG( "Rank %d = %.3f", scores[i].first, scores[i].second );
  }

  return scores.front().first;
}

int RankClassifier::DetectCurrentRank() const {
  QImage raw = Hearthstone::Instance()->Capture( RC_CAPTURE_SCREEN_WIDTH, RC_CAPTURE_SCREEN_HEIGHT,
      RC_CAPTURE_X, RC_CAPTURE_Y,
      RC_LABEL_WIDTH, RC_LABEL_HEIGHT ).toImage();

  QImage label = raw.scaled( QSize( RC_LABEL_WIDTH, RC_LABEL_HEIGHT ),
    Qt::IgnoreAspectRatio,
    Qt::SmoothTransformation );

  return Classify( label );
}
