#pragma once

#include <QApplication>
#include <QLocalServer>

#include "ResultTracker.h"
#include "WebProfile.h"
#include "ui/Window.h"

class Trackobot : public QObject
{
  Q_OBJECT

private:
  QApplication mApp;
  Window *mWindow;
  QLocalServer *mSingleInstanceServer;

  ResultTracker *mResultTracker;
  WebProfile *mWebProfile;

  bool IsAlreadyRunning();

  void SetupApplication();
  void SetupLogging();
  void SetupUpdater();

  void CreateUI();
  void Initialize();

public:
  Trackobot( int argc, char **argv );
  ~Trackobot();
  int Run();
};
