#pragma once

#include "window_capture.h"

#include <ApplicationServices/ApplicationServices.h>
#include <QElapsedTimer>
#include <QtMacExtras>

// FindWindow is quite intensive in terms of performance
#define OSX_UPDATE_WINDOW_DATA_INTERVAL 1000 // ms

class OSXWindowCapture : public WindowCapture
{
protected:

  string name;
  int winId;
  CGRect rect;

  static int FindWindow(const string& name);
  static CGRect GetWindowRect(int windowId);

  bool IsFullscreen();

  QElapsedTimer updateTimer;
  void Update(bool forced = false);

public:
  OSXWindowCapture(const string& windowName);

  bool WindowFound();
  int GetWidth();
  int GetHeight();

  QPixmap Capture(int x, int y, int w = 0, int h = 0);
};
