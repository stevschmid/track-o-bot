#include "NeuralNetwork.h"

#include <math.h>

#include <cassert>
#include <numeric>

namespace NN
{
  NeuralNetwork::NeuralNetwork() {
  }

  NeuralNetwork::NeuralNetwork( const Vector& hiddenLayerBias, const Matrix& hiddenLayerWeights,
      const Vector& outputLayerBias, const Matrix& outputLayerWeights )
  : mHiddenLayerBias( hiddenLayerBias ), mHiddenLayerWeights( hiddenLayerWeights ),
    mOutputLayerBias( outputLayerBias ), mOutputLayerWeights( outputLayerWeights )
  {
  }

  Vector NeuralNetwork::FeedForward( const Vector& input, const Vector& layerBias, const Matrix& layerWeights ) const {
    int numNodes = ( int )layerBias.size();

    Vector responses( numNodes );
    for( int nodeIdx = 0; nodeIdx < numNodes; nodeIdx++ ) {
      const Vector& weights = layerWeights[ nodeIdx ];
      float bias = layerBias[ nodeIdx ];

      assert( weights.size() == input.size() );

      // z = b + w * x
      float t = bias + std::inner_product( input.begin(), input.end(), weights.begin(), 0.0f );
      float y = 1.0f / ( 1.0f + expf(-t) );

      responses[ nodeIdx ] = y;
    }

    return responses;
  }

  Vector NeuralNetwork::Run( const Vector& input ) const {
    assert( mHiddenLayerWeights.size() > 0 && input.size() == mHiddenLayerWeights[ 0 ].size() );

    Vector hidden = FeedForward( input, mHiddenLayerBias, mHiddenLayerWeights );
    Vector output = FeedForward( hidden, mOutputLayerBias, mOutputLayerWeights );
    return output;
  }
}
