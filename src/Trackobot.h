#pragma once

#include <QApplication>
#include <QLocalServer>

#include "Core.h"
#include "ui/Window.h"

class Trackobot : public QObject
{
  Q_OBJECT

private:
  QApplication mApp;
  Core *mCore;
  Window *mWindow;
  QLocalServer *mSingleInstanceServer;

  WebProfile mWebProfile;

  bool IsAlreadyRunning();

  void SetupApplication();
  void SetupLogging();
  void SetupUpdater();

  void CreateCore();
  void CreateUI();
  void Initialize();

public:
  Trackobot( int argc, char **argv );
  ~Trackobot();
  int Run();
};
