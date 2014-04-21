#pragma once

#include "window_capture.h"

#include <ApplicationServices/ApplicationServices.h>

class OSXWindowCapture : public WindowCapture
{
protected:

  string name;
  int winId;
  CGRect rect;

  static int FindWindow(const string& name);
  static CGRect GetWindowRect(int windowId);

  bool IsFullscreen();
  void Update();

public:
  OSXWindowCapture(const string& windowName);

  bool WindowFound();
  int GetWidth();
  int GetHeight();

  QPixmap Capture(int x, int y, int w = 0, int h = 0);
};
