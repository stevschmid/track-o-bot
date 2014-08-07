#include "Phash.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <xmmintrin.h>

#if defined(_MSC_VER)
#define ALIGN(x,t) __declspec(align(x)) t
#else
#if defined(__GNUC__)
#define ALIGN(x,t) t __attribute__ ((aligned(x)))
#endif
#endif

Phash::Phash( quint64 hash )
  : mHash( hash )
{
}

Phash::Phash( const QPixmap& pixmap )
  : mHash( CalculateHashForPixmap( pixmap ) )
{
}

quint64 Phash::CalculateHashForPixmap( const QPixmap& pixmap ) {
  static bool tableInitialized = false;

  ALIGN( 16, static float cosTable[ 8 ][ 8 ][ 32 ][ 32 ] );
  ALIGN( 16, float intensity[ 32 ][ 32 ] );

  if( !tableInitialized ) {
    tableInitialized = true;

    // 32x32 DCT, though we are only interested in the top left 8x8, representing lowest frequencies in the image
    for( int u = 0; u < 8; u++ ) {
      for( int v = 0; v < 8; v++ ) {
        for( int y = 0; y < 32; y++ ) {
          for( int x = 0; x < 32; x++ ) {
            cosTable[ v] [ u ][ y ][ x ] =   cosf( M_PI / 32.0f * ( x + 0.5f ) * u )
                                           * cosf( M_PI / 32.0f * ( y + 0.5f ) * v );
          }
        }
      }
    }
  }

  // Scale down to 32x32
  QImage image = pixmap.scaled( 32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ).toImage();

  // Convert to grayscale
  const __m128 luminance = _mm_set_ps( .0f, 0.2126f, 0.7152f, 0.0722f );

  for( int y = 0; y < 32; y++ ) {
    for( int x = 0; x < 32; x++ ) {
      QRgb pixel = image.pixel( x, y );

      __m128 p = _mm_set_ps( 0, qRed( pixel ), qGreen( pixel ), qBlue( pixel ) );
      __m128 v = _mm_mul_ps( luminance, p );
      __m128 t = _mm_add_ps( v, _mm_movehl_ps( v, v ) );
      __m128 sum = _mm_add_ss( t, _mm_shuffle_ps( t, t, 1 ) );
      _mm_store_ss( &intensity[ y ][ x ], sum );
    }
  }

  // DCT
  float dct[ 64 ];
  int counter = 0;

  for( int u = 0; u < 8; u++ ) {
    for( int v = 0; v < 8; v++ ) {
      __m128 acc = _mm_setzero_ps();

      for( int y = 0; y < 32; y++ ) {
        for( int x = 0; x < 32; x+=4 ) {
          __m128 in = _mm_load_ps( &intensity[ y ][ x ] );
          __m128 cos = _mm_load_ps( &cosTable[ v ][ u ][ y ][ x ] );
          __m128 out = _mm_mul_ps( in, cos );
          acc = _mm_add_ps(out, acc);
        }
      }

      __m128 t = _mm_add_ps( acc, _mm_movehl_ps( acc, acc ) );
      __m128 sum = _mm_add_ss( t, _mm_shuffle_ps( t, t, 1 ) );
      _mm_store_ss( &dct[ counter++ ], sum );
    }
  }

  // Mean, skip first one
  float mean = 0.0;
  for( int i = 1; i < 64; i++ ) {
    mean += dct[ i ];
  }
  mean /= 63;

  // Calculate the final hash
  quint64 hash = 0;

  for( int i = 0; i < 64; i++ ) {
    quint64 val = dct[ i ] > mean;
    hash |= val << i;
  }

  return hash;
}

unsigned int Phash::CalculateHammingDistance( const Phash& otherHash ) const {
  unsigned int dist = 0;
  quint64 diff = Hash() ^ otherHash.Hash();

  while( diff ) {
    ++dist;
    diff &= diff - 1;
  }

  return dist;
}

bool Phash::operator==( const Phash& otherHash ) const {
  return CalculateHammingDistance( otherHash ) <= PHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD;
}

quint64 Phash::Hash() const {
  return mHash;
}
