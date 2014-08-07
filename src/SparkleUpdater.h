#pragma once

#include "Updater.h"

#include <QString>

class SparkleUpdater : public Updater {
private:
  class Private;
  Private* d;

public:
  SparkleUpdater( const QString& url );
  ~SparkleUpdater();

  virtual void CheckForUpdatesNow();

  virtual void SetAutomaticallyChecksForUpdates( bool automaticallyChecks );
  virtual bool AutomaticallyChecksForUpdates();
};
