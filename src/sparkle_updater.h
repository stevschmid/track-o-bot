#pragma once

#include "updater.h"

#include <QString>

class SparkleUpdater : public Updater {
public:
  SparkleUpdater(const QString& url);
  ~SparkleUpdater();

  virtual void setAutomaticallyChecksForUpdates(bool automaticallyChecks);
  virtual bool automaticallyChecksForUpdates();
  virtual void checkForUpdatesNow();

private:
  class Private;
  Private* d;
};
