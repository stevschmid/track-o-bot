#pragma once

#include "WindowCapture.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WinWindowCapture : public WindowCapture
{
private:
  QString mWindowName;

  RECT Rect();
  HWND FindHWND();

public:
  WinWindowCapture( const QString& windowName );

  bool WindowFound();

  int Width();
  int Height();

  QPixmap Capture( int x, int y, int w, int h );
};

