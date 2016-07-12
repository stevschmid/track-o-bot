#pragma once

#include <QPixmap>
#include <QTimer>

#include "WindowCapture.h"

const int NUM_LOG_MODULES = 5;
const char LOG_MODULE_NAMES[ NUM_LOG_MODULES ][ 32 ] = {
  "Zone",
  "Asset",
  "Bob",
  "Power",
  "LoadingScreen"
};

class Hearthstone : public QObject
{
  Q_OBJECT

  DEFINE_SINGLETON( Hearthstone );

private:
  WindowCapture *mCapture;

  bool mRestartRequired; // in case HS needs to be restarted for log changes to take effect
  bool mGameRunning;
  bool mGameHasFocus;

  QString ReadAgentAttribute( const char *attributeName ) const;

  QTimer *mTimer;

public:
  // Allow to override window capture for test environment
  void SetWindowCapture( WindowCapture *windowCapture );

  bool GameRunning() const;
  QPixmap Capture( int canvasWidth, int canvasHeight, int cx, int cy, int cw, int ch  );
  bool CaptureWholeScreen( QPixmap *screen );

  void EnableLogging();
  void DisableLogging();

  QString LogConfigPath() const;
  QString DetectHearthstonePath() const;
  QString DetectRegion() const;

  int Width() const;
  int Height() const;

  bool HasFocus() const;

signals:
  void GameStarted();
  void GameStopped();
  void GameRequiresRestart();
  void GameWindowChanged( int x, int y, int w, int h );
  void FocusChanged( bool focus );

private slots:
  void Update();
};
