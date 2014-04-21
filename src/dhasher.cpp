#include "dhasher.h"

DEFINE_SINGLETON_SCOPE(DHasher)

DHasher::DHasher() {

}

DHasher::~DHasher() {

}

unsigned int DHasher::CalculateHammingDistance(dhash v1, dhash v2) {
  unsigned int dist = 0;
  dhash diff = v1 ^ v2;

  while(diff) {
    ++dist;
    diff &= diff - 1;
  }

  return dist;
}

dhash DHasher::HashOfPixmap(const QPixmap& pixmap) {
  dhash hash = 0;

  QImage image = pixmap.scaled(9, 8, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage();

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

bool DHasher::Similar(dhash v1, dhash v2) {
  return CalculateHammingDistance(v1, v2) <= DHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD;
}
