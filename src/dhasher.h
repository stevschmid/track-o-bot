#pragma once

typedef quint64 dhash;

//
// C++ implementation of Dhash as outlined here: http://www.hackerfactor.com/blog/?/archives/529-Kind-of-Like-That.html
//

#define DHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD 10 // bits

class DHasher
{
  DEFINE_SINGLETON(DHasher)

public:
  unsigned int CalculateHammingDistance(dhash v1, dhash v2);
  dhash HashOfPixmap(const QPixmap& pixmap);

  bool Similar(dhash v1, dhash v2);
};
