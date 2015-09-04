#pragma once

#include <QApplication>

#include "Core.h"
#include "ui/Window.h"

class Trackobot : public QObject
{
  Q_OBJECT

private:
  QApplication mApp;
  Core *mCore;
  Window *mWindow;

  bool IsAlreadyRunning() const;

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
