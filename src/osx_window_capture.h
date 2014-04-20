#pragma once

#include "window_capture.h"

class OSXWindowCapture : public WindowCapture
{
protected:
  string m_windowTitle;

public:
  OSXWindowCapture(const string& windowTitle);

  int getWidth();
  int getHeight();

  QPixmap capture(int x, int y, int w = 0, int h = 0);
};
