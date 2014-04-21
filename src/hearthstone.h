#pragma once

#include "osx_window_capture.h"

// Hearthstone provides a virtual canvas of 1024x768, which is independent
// of the resolution set

#define VIRTUAL_CANVAS_WIDTH 1024
#define VIRTUAL_CANVAS_HEIGHT 768

typedef enum {
  GRAVITY_CENTER
} Gravity;

class Hearthstone
{
protected:
  WindowCapture *capture;

public:
  Hearthstone();
  ~Hearthstone();

  bool IsRunning();
  QPixmap Capture(int vx, int vy, int vw, int vh, Gravity gravity = GRAVITY_CENTER);
};


