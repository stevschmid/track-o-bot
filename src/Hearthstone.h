#pragma once

#include <QPixmap>
#include <QDir>

#include "WindowCapture.h"

// Hearthstone provides a virtual canvas of 1024x768, which is independent
// of the resolution set

#define VIRTUAL_CANVAS_WIDTH 1024
#define VIRTUAL_CANVAS_HEIGHT 768

class Hearthstone
{
  DEFINE_SINGLETON( Hearthstone );

private:
  WindowCapture *mCapture;

  bool mRestartRequired; // in case HS needs to be restarted for log changes to take effect

public:
  // Allow to override window capture for test environment
  void SetWindowCapture( WindowCapture *windowCapture );

  bool Running();
  QPixmap Capture( int vx, int vy, int vw, int vh );

  void EnableLogging();
  void DisableLogging();

  void SetRestartRequired( bool restartRequired );
  bool RestartRequired() const;

  string LogConfigPath();
  string LogPath();

  int Width();
  int Height();

};
