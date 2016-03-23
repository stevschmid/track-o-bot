#pragma once

#include <QApplication>
#include <QLocalServer>

#include "ResultTracker.h"
#include "WebProfile.h"
#include "HearthstoneLogTracker.h"

#include "ui/Window.h"
#include "ui/Overlay.h"

class Trackobot : public QApplication
{
  Q_OBJECT

private:
  Window *mWindow;
  Overlay *mOverlay;
  QLocalServer *mSingleInstanceServer;

  ResultTracker *mResultTracker;
  WebProfile *mWebProfile;
  HearthstoneLogTracker *mLogTracker;

  bool IsAlreadyRunning();

  void SetupApplication();
  void SetupLogging();
  void SetupUpdater();

  void CreateUI();
  void WireStuff();
  void Initialize();

public:
  Trackobot( int& argc, char **argv );
  ~Trackobot();
  int Run();
};
