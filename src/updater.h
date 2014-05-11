#pragma once

// https://el-tramo.be/blog/mixing-cocoa-and-qt/

class Updater {
public:
  virtual ~Updater() {}
  virtual void checkForUpdates() = 0;
};
