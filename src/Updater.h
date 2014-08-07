#pragma once

// https://el-tramo.be/blog/mixing-cocoa-and-qt/

class Updater {
public:
  virtual ~Updater() {}

  virtual void CheckForUpdatesNow() = 0;

  virtual void SetAutomaticallyChecksForUpdates( bool automaticallyChecks ) = 0;
  virtual bool AutomaticallyChecksForUpdates() = 0;
};
