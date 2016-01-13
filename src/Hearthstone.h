#pragma once

#include <QPixmap>
#include <QTimer>

#include "WindowCapture.h"

const int NUM_LOG_MODULES = 4;
const char LOG_MODULE_NAMES[ NUM_LOG_MODULES ][ 32 ] = {
  "Zone",
  "Asset",
  "Bob",
  "Power"
};

class Hearthstone : public QObject
{
  Q_OBJECT

  DEFINE_SINGLETON( Hearthstone )

private:
  WindowCapture *mCapture;

  bool mRestartRequired; // in case HS needs to be restarted for log changes to take effect
  bool mIsRunning;

  QString ReadAgentAttribute( const char *attributeName ) const;
  QString WindowName() const;

  QTimer *mTimer;

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

signals:
  void GameStarted();
  void GameStopped();

private slots:
  void Update();


};
