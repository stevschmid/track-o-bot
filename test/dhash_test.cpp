#include "dhash.h"
#include "gtest/gtest.h"

#include <QApplication>

TEST(DHashTest, HammingTrivial) {
  dhash v1 = 0b1001;
  dhash v2 = 0b0101;
  EXPECT_EQ(dhash_calculate_hamming_distance(v1, v2), 2);
}

TEST(DHashTest, HammingFull64BitRange) {
  dhash v1 = (dhash)0x0300000000000010;
  dhash v2 = (dhash)0x0400000000000020;
  EXPECT_EQ(dhash_calculate_hamming_distance(v1, v2), 5);
}

TEST(DHashTest, HashOfSimplePixmap) {
  int argc = 0;
  char **argv = NULL;
  QApplication app(argc, argv);
  QImage test(9,8, QImage::Format_RGB888);

  QRgb white = QColor(255, 255, 255).rgb();
  QRgb black = QColor(0, 0, 0).rgb();

  test.fill(black);

  test.setPixel(1, 0, white);
  test.setPixel(2, 1, white);
  test.setPixel(3, 2, white);
  test.setPixel(4, 3, white);
  test.setPixel(5, 4, white);
  test.setPixel(6, 5, white);
  test.setPixel(7, 6, white);
  test.setPixel(8, 7, white);

  dhash value = dhash_for_pixmap(QPixmap::fromImage(test));

  EXPECT_EQ(value, 0x8040201008040201);
}
