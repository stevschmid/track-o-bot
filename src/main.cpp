#include <QApplication.h>

#include "hearthstone.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  Hearthstone hs;
  QPixmap pixmap = hs.Capture(470, 281, 50, 20);
  pixmap.save("debug.png");

  return 0;
}
