#pragma once

#include <QImage>

#include "NeuralNetwork.h"

class RankClassifier
{
private:
  NN::NeuralNetwork mNeuralNetwork;

  NN::NeuralNetwork BuildNN() const;

public:
  RankClassifier();
  int Classify( const QImage& label ) const;
  int DetectCurrentRank() const;
};
