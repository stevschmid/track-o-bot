#include "phash.h"

#include <math.h>
#include <xmmintrin.h>

phash phash_for_pixmap(const QPixmap& pixmap) {
  static float cos_table[8][8][32][32];
  static bool cos_table_initialized = false;

  if(!cos_table_initialized) {
    cos_table_initialized = true;

    // 32x32 DCT, though we are only interested in the top left 8x8, representing lowest frequencies in the image
    for(int u = 0; u < 8; u++) {
      for(int v = 0; v < 8; v++) {
        for(int y = 0; y < 32; y++) {
          for(int x = 0; x < 32; x++) {
            cos_table[v][u][y][x] = cosf(M_PI / 32.0f * (x + 0.5f) * u)
                                  * cosf(M_PI / 32.0f * (y + 0.5f) * v);
          }
        }
      }
    }
  }

  // Scale down to 32x32
  QImage image = pixmap.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage();

  float dct[64];
  int counter = 0;
  float intensity[32][32];

  // Convert to grayscale
  const __m128 luminance = _mm_set_ps(.0f, 0.2126f, 0.7152f, 0.0722f);

  for(int y = 0; y < 32; y++) {
    for(int x = 0; x < 32; x++) {
      QRgb pixel = image.pixel(x, y);

      __m128 p = _mm_set_ps(0, qRed(pixel), qGreen(pixel), qBlue(pixel));
      __m128 v = _mm_mul_ps(luminance, p);
      __m128 t = _mm_add_ps(v, _mm_movehl_ps(v, v));
      __m128 sum = _mm_add_ss(t, _mm_shuffle_ps(t, t, 1));
      _mm_store_ss(&intensity[y][x], sum);
    }
  }

  // DCT
  for(int u = 0; u < 8; u++) {
    for(int v = 0; v < 8; v++) {
      __m128 acc = _mm_setzero_ps();

      for(int y = 0; y < 32; y++) {
        for(int x = 0; x < 32; x+=4) {
          __m128 in = _mm_load_ps(&intensity[y][x]);
          __m128 cos = _mm_load_ps(&cos_table[v][u][y][x]);
          __m128 out = _mm_mul_ps(in, cos);
          acc = _mm_add_ps(out, acc);
        }
      }

      __m128 t = _mm_add_ps(acc, _mm_movehl_ps(acc, acc));
      __m128 sum = _mm_add_ss(t, _mm_shuffle_ps(t, t, 1));
      _mm_store_ss(&dct[counter++], sum);
    }
  }

  // Mean, skip first one
  float mean = 0.0;
  for(int i = 1; i < 64; i++) {
    mean += dct[i];
  }
  mean /= 63;

  // Calculate the final hash
  phash hash = 0;

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

