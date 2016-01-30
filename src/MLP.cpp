#include "MLP.h"

#include <math.h>

#include <cassert>
#include <numeric>
#include <algorithm>

namespace MLP
{
  MultiLayerPerceptron::MultiLayerPerceptron() {
  }

  void MultiLayerPerceptron::AddLayer( const Layer& layer ) {
    mLayers.push_back( layer );
  }

  Vector MultiLayerPerceptron::FeedForward( const Vector& input, const Layer& layer ) const {
    int numNodes = layer.biases.size();

    Vector z( numNodes );
    for( int i = 0; i < numNodes; i++ ) {
      const Vector& weights = layer.weights[ i ];
      float bias = layer.biases[ i ];

      assert( weights.size() == input.size() );

      // z = wT * x + b
      z[ i ] = std::inner_product( weights.begin(), weights.end(), input.begin(), bias );
    }

    Vector output( numNodes );
    if( layer.type == LAYER_SIGMOID ) {
      for( int i = 0; i < numNodes; i++ ) {
        output[ i ] = 1.0f / ( 1.0f + expf( -z[ i ] ) );
      }
    } else if( layer.type == LAYER_RECTIFIER ) {
      for( int i = 0; i < numNodes; i++ ) {
        output[ i ] = std::max( 0.0f, z[ i ] );
      }
    } else if( layer.type == LAYER_SOFTMAX ) {
      // Use the log-sum trick to compute the exponential sum
      // oi = exp(zi) / sum_j { exp(zj) }
      // log oi = zi - log( sum_j { exp(zj) } )
      // log oi = zi - log( sum_j { exp(zj) - m + m } )
      // log oi = zi - m - log( sum_j { exp(zj) - m } )
      float m = *std::max_element( z.begin(), z.end() );
      float logSum = logf(
        std::accumulate( z.begin(), z.end(), 0.0f, [m]( float sum, float zj ) {
          return sum + expf( zj - m );
        })
      );

      for( int i = 0; i < numNodes; i++ ) {
        output[ i ] = expf( z[ i ] - m - logSum );
      }
    }

    return output;
  }

  Vector MultiLayerPerceptron::Compute( const Vector& input ) const {
    Vector res = input;

    assert( mLayers.size() );
    for( const Layer& layer : mLayers ) {
      res = FeedForward( res, layer );
    }

    return res;
  }
}
