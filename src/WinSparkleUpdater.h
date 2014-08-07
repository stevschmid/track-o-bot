#pragma once

#include "Updater.h"

#include <QString>

class WinSparkleUpdater : public Updater {
public:
  WinSparkleUpdater( const QString& url );
  ~WinSparkleUpdater();

  virtual void CheckForUpdatesNow();

  virtual void SetAutomaticallyChecksForUpdates( bool automaticallyChecks );
  virtual bool AutomaticallyChecksForUpdates();
};
