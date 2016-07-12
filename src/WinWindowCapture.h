#pragma once

#include "WindowCapture.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WinWindowCapture : public WindowCapture
{
private:
  RECT Rect();
  POINT ScreenPos();

  HWND FindHWND();

  HWND mHwnd;

public:
  WinWindowCapture();

  bool WindowFound();

  int Width();
  int Height();

  int Left();
  int Top();

  QPixmap Capture( int x, int y, int w, int h );

  bool HasFocus();
};

