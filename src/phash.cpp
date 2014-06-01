#include "phash.h"

#include <math.h>

phash phash_for_pixmap(const QPixmap& pixmap) {
  phash hash = 0;

  QImage image = pixmap.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage();

  double dct[64];
  int counter = 0;

  for(int u = 0; u < 8; u++) {
    for(int v = 0; v < 8; v++) {
      double acc = 0.0;

      for(int x = 0; x < 32; x++) {
        for(int y = 0; y < 32; y++) {
          QRgb thisPixel = image.pixel(x, y);
          acc += qGray(thisPixel)
            * cos(M_PI / 32.0 * (x + 0.5) * u)
            * cos(M_PI / 32.0 * (y + 0.5) * v);
        }
      }

      dct[counter++] = acc;
    }
  }

  double mean = 0.0;
  for(int i = 1; i < 64; i++) {
    mean += dct[i];
  }
  mean /= 63;

  for(int i = 0; i < 64; i++) {
    phash val = dct[i] > mean;
    hash |= val << i;
  }

  return hash;
}

unsigned int phash_calculate_hamming_distance(phash v1, phash v2) {
  unsigned int dist = 0;
  phash diff = v1 ^ v2;

  while(diff) {
    ++dist;
    diff &= diff - 1;
  }

  return dist;
}

bool phash_check_similarity(phash v1, phash v2) {
  return phash_calculate_hamming_distance(v1, v2) <= PHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD;
}

