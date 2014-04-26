#include "dhash.h"

dhash dhash_for_pixmap(const QPixmap& pixmap) {
  dhash hash = 0;

  QImage image = pixmap.scaled(9, 8, Qt::IgnoreAspectRatio, Qt::FastTransformation).toImage();

  int w = image.width();
  int h = image.height();

  int bitCounter = 0;

  for(int y = 0; y < h; y++) {
    for(int x = 0; x < w - 1; x++) {
      QRgb thisPixel = image.pixel(x, y);
      QRgb nextPixel = image.pixel(x + 1, y);

      int thisGray = qGray(thisPixel);
      int nextGray = qGray(nextPixel);

      if(thisGray < nextGray) {
        hash |= (dhash(1) << bitCounter);
      }

      bitCounter++;
    }
  }

  return hash;
}

unsigned int dhash_calculate_hamming_distance(dhash v1, dhash v2) {
  unsigned int dist = 0;
  dhash diff = v1 ^ v2;

  while(diff) {
    ++dist;
    diff &= diff - 1;
  }

  return dist;
}

bool dhash_check_similarity(dhash v1, dhash v2) {
  return dhash_calculate_hamming_distance(v1, v2) <= DHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD;
}
