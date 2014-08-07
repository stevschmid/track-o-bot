#pragma once

#include <QPixmap>

class WindowCapture
{
public:
  virtual ~WindowCapture() {}

  virtual bool WindowFound() = 0;

  virtual int Width() = 0;
  virtual int Height() = 0;

  virtual QPixmap Capture( int x, int y, int w, int h ) = 0;
};

