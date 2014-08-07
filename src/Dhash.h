#pragma once

#include <QPixmap>

// Implementation of Dhash as outlined here: http://www.hackerfactor.com/blog/?/archives/529-Kind-of-Like-That.html

#define DHASH_HAMMING_DISTANCE_SIMILAR_THRESHOLD 10 // bits

typedef quint64 dhash;

dhash         dhash_for_pixmap(const QPixmap& pixmap);
unsigned int  dhash_calculate_hamming_distance(dhash v1, dhash v2);
bool          dhash_check_similarity(dhash v1, dhash v2);
