#include <QApplication.h>

#include "hearthstone.h"
#include "dhash.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  Hearthstone hs;

  DHash calc;

  QPixmap pixmap1 = hs.Capture(470, 281, 50, 20);
  dhash value1 = calc.ForPixmap(pixmap1);
  std::cout << "Value 1 " << std::hex << value1 << endl;
  sleep(1);

  QPixmap pixmap2 = hs.Capture(470, 281, 50, 20);
  dhash value2 = calc.ForPixmap(pixmap2);
  std::cout << "Value 2 " << std::hex << value2 << endl;

  std::cout << "Hamming Distance " << calc.CalculateHammingDistance(value1, value2) << endl;

  return 0;
}
