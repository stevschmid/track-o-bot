#pragma once

#include "window_capture.h"

#include <ApplicationServices/ApplicationServices.h>

// remove the window title bar which we are not interested in
#define OSX_WINDOW_TITLE_BAR_HEIGHT 22

class OSXWindowCapture : public WindowCapture
{
protected:

  string name;
  int winId;
  CGRect rect;

  static int FindWindow(const string& name);
  static CGRect GetWindowRect(int windowId);

public:

  OSXWindowCapture(const string& windowName);

  void Update();

  int GetWidth();
  int GetHeight();

  QPixmap Capture(int x, int y, int w = 0, int h = 0);

};
