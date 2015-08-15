#pragma once

#include <QImage>

#include "MLP.h"

class RankClassifier
{
private:
  MLP::NeuralNetwork mNeuralNetwork;

  static MLP::NeuralNetwork BuildMLP();
  static MLP::Vector BinarizeImage( const QImage& img, float threshold );

public:
  RankClassifier();
  int Classify( const QImage& label ) const;
  int DetectCurrentRank() const;
};
