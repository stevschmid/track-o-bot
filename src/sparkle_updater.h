#pragma once

#include "updater.h"

#include <QString>

class SparkleUpdater : public Updater {
public:
  SparkleUpdater(const QString& url);
  ~SparkleUpdater();
  virtual void checkForUpdates();

private:
  class Private;
  Private* d;
};
