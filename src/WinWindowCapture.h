#pragma once

#include "WindowCapture.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WinWindowCapture : public WindowCapture
{
private:
  string name;

  RECT GetRect();
  HWND FindHWND();

public:
  WinWindowCapture( const string& windowName );

  bool WindowFound();

  int GetWidth();
  int GetHeight();

  QPixmap Capture( int x, int y, int w, int h );
};

