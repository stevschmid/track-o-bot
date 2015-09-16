#pragma once

#include <QImage>

#include "MLP.h"

class RankClassifier
{
private:
  MLP::MultiLayerPerceptron mMLP;

  static MLP::Vector BinarizeImageSV( const QImage& img, float maxSaturation, float minValue );

  void LoadMLP();

public:
  RankClassifier();
  int Classify( const QImage& label ) const;
  int DetectCurrentRank() const;
};
