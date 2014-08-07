#pragma once

#include <QPixmap>

class WindowCapture
{
public:
  virtual ~WindowCapture() {}

  virtual bool WindowFound() = 0;

  virtual int GetWidth() = 0;
  virtual int GetHeight() = 0;

  virtual QPixmap Capture(int x, int y, int w, int h) = 0;
};

