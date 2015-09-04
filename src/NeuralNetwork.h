#pragma once

#include <vector>

namespace NN
{
  typedef std::vector< float > Vector;
  typedef std::vector< Vector > Matrix;

  // Simple predicting feed forward neural network with 1 hidden layer
  // (using previously trained parameters)
  class NeuralNetwork
  {
  private:
    Vector mHiddenLayerBias;
    Matrix mHiddenLayerWeights;

    Vector mOutputLayerBias;
    Matrix mOutputLayerWeights;

    Vector FeedForward( const Vector& input, const Vector& layerBias, const Matrix& layerWeights ) const;

  public:
    NeuralNetwork();
    NeuralNetwork( const Vector& hiddenLayerBias, const Matrix& hiddenLayerWeights,
                   const Vector& outputLayerBias, const Matrix& outputLayerWeights );

    Vector Run( const Vector& input ) const;
  };
}
