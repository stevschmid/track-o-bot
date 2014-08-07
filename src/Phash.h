#pragma once

#include <QPixmap>

// Implementation of Phash as outlined here:
// http://www.hackerfactor.com/blog/index.php?/archives/432-Looks-Like-It.html

#define PHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD 10 // bits

class Phash
{
private:
  quint64 mHash;

  quint64 CalculateHashForPixmap( const QPixmap& pixmap );

public:
  Phash( quint64 hash = 0 );
  Phash( const QPixmap& pixmap );

  quint64 Hash() const;
  bool operator==( const Phash& otherPhash ) const;
  unsigned int CalculateHammingDistance( const Phash& otherPhash ) const;
};

