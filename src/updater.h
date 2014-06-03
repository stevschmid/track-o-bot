#pragma once

// https://el-tramo.be/blog/mixing-cocoa-and-qt/

class Updater {
public:
  virtual ~Updater() {}

  virtual void setAutomaticallyChecksForUpdates(bool automaticallyChecks) = 0;
  virtual bool automaticallyChecksForUpdates() = 0;
};
