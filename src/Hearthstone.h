#pragma once

#include <QPixmap>

#include "WindowCapture.h"

const int NUM_LOG_MODULES = 4;
const char LOG_MODULE_NAMES[ NUM_LOG_MODULES ][ 32 ] = {
  "Zone",
  "Asset",
  "Bob",
  "Power"
};

class Hearthstone
{
  DEFINE_SINGLETON( Hearthstone )

private:
  WindowCapture *mCapture;

  bool mRestartRequired; // in case HS needs to be restarted for log changes to take effect

  QString ReadAgentAttribute( const char *attributeName ) const;
  QString WindowName() const;

public:
  // Allow to override window capture for test environment
  void SetWindowCapture( WindowCapture *windowCapture );

  bool Running() const;
  QPixmap Capture( int canvasWidth, int canvasHeight, int cx, int cy, int cw, int ch  );
  bool CaptureWholeScreen( QPixmap *screen );

  void EnableLogging();
  void DisableLogging();

  void SetRestartRequired( bool restartRequired );
  bool RestartRequired() const;

  QString LogConfigPath() const;
  QString LogPath( const QString& fileName ) const;

  int Width() const;
  int Height() const;
};
