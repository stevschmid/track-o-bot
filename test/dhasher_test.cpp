#include <limits.h>
#include "dhasher.h"
#include "gtest/gtest.h"

TEST(DHasherTest, Trivial) {
  dhash v1 = 0b1001;
  dhash v2 = 0b0101;
  EXPECT_EQ(DHasher::Instance()->CalculateHammingDistance(v1, v2), 2);
}

TEST(DHasherTest, Full64BitRange) {
  dhash v1 = (dhash)0x0300000000000010;
  dhash v2 = (dhash)0x0400000000000020;
  EXPECT_EQ(DHasher::Instance()->CalculateHammingDistance(v1, v2), 5);
}
