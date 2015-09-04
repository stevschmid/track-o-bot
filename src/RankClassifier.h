#pragma once

#include <QImage>

#include "NeuralNetwork.h"

class RankClassifier
{
private:
  NN::NeuralNetwork mNeuralNetwork;

  static NN::NeuralNetwork BuildNN();
  static NN::Vector BinarizeImage( const QImage& img, float threshold );

public:
  RankClassifier();
  int Classify( const QImage& label ) const;
  int DetectCurrentRank() const;
};
