#pragma once

#include <QPixmap>

class WindowCapture
{
public:
  virtual ~WindowCapture() {}

  virtual int getWidth() = 0;
  virtual int getHeight() = 0;

  virtual QPixmap capture(int x, int y, int w = 0, int h = 0) = 0;
};

