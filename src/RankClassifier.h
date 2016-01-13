#pragma once

#include <QImage>

#include "MLP.h"

class RankClassifier
{
private:
  MLP::MultiLayerPerceptron mMLP;

  static MLP::Vector BinarizeImageSV( const QImage& img, float maxSaturation, float minValue );

  void LoadMLP();
  int Classify( const QImage& label, float *outScore ) const;

public:
  RankClassifier();
  int DetectCurrentRank( float *outScore, QImage *outLabel );
};
