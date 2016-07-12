#pragma once

#include <QPixmap>

class WindowCapture
{
public:
  virtual ~WindowCapture() {}

  virtual bool WindowFound() = 0;

  virtual int Width() = 0;
  virtual int Height() = 0;

  virtual int Left() = 0;
  virtual int Top() = 0;

  virtual QPixmap Capture( int x, int y, int w, int h ) = 0;

  virtual bool HasFocus() = 0;
};

