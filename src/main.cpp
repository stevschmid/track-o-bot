#include "osx_window_capture.h"

int main()
{
  WindowCapture *capture = new OSXWindowCapture("Hearthstone");
  delete capture;

  return 0;
}
