#pragma once

#include <QPixmap>

#include "window_capture.h"

// Hearthstone provides a virtual canvas of 1024x768, which is independent
// of the resolution set

#define VIRTUAL_CANVAS_WIDTH 1024
#define VIRTUAL_CANVAS_HEIGHT 768

class Hearthstone
{
  DEFINE_SINGLETON(Hearthstone)

private:
  WindowCapture *capture;

public:
  // Allow to override window capture for test environment
  void SetWindowCapture(WindowCapture *wc);

  bool IsRunning();
  QPixmap Capture(int vx, int vy, int vw, int vh);

  void EnableLogging();
  void DisableLogging();

  string LogConfigPath();
  string LogPath();

};
