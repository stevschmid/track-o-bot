#include "osx_window_capture.h"

OSXWindowCapture::OSXWindowCapture(const string& windowTitle)
{
  m_windowTitle = windowTitle;
}

int OSXWindowCapture::getWidth()
{
  return 0;
}

int OSXWindowCapture::getHeight()
{
  return 0;
}

QPixmap OSXWindowCapture::capture(int x, int y, int w, int h)
{
  return QPixmap();
}
