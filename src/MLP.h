#pragma once

#include <vector>

namespace MLP
{
  typedef std::vector< float > Vector;
  typedef std::vector< Vector > Matrix;

  typedef enum {
    LAYER_SIGMOID,
    LAYER_SOFTMAX,
    LAYER_RECTIFIER
  } LayerType;

  typedef struct {
    LayerType   type;
    Vector      biases;
    Matrix      weights;
  } Layer;

  // Simple feed forward Artificial Neural Network
  // Only for prediction, using pretrained parameters
  class MultiLayerPerceptron {
  private:
    std::vector< Layer > mLayers;

    Vector FeedForward( const Vector& input, const Layer& layer ) const;

  public:
    MultiLayerPerceptron();
    void AddLayer( const Layer& layer );

    Vector Compute( const Vector& input ) const;
  };
}
