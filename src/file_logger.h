#pragma once

#include "logger.h"

#include <fstream>

class FileLogger : public LoggingObserver {
protected:
  ofstream of;

public:
  FileLogger(const string& path) {
    of.open(path);
  }
  ~FileLogger() {}

  void HandleLogEntry(const string& entry) {
    of << entry;
  }
};

