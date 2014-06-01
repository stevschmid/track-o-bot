#pragma once

#include <QPixmap>

// Implementation of Dhash as outlined here:
// http://www.hackerfactor.com/blog/index.php?/archives/432-Looks-Like-It.html

#define PHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD 10 // bits

typedef quint64 phash;

phash         phash_for_pixmap(const QPixmap& pixmap);
unsigned int  phash_calculate_hamming_distance(phash v1, phash v2);
bool          phash_check_similarity(phash v1, phash v2);

