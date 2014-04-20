#include "osx_window_capture.h"

#include <QApplication.h>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  WindowCapture *capture = new OSXWindowCapture("Hearthstone");
  cout << "Width " << capture->GetWidth() << endl;
  cout << "Height " << capture->GetHeight() << endl;

  QPixmap pixmap = capture->Capture(0, 0);
  pixmap.save("debug.png");

  delete capture;

  return 0;
}
