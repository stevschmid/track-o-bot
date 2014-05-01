#pragma once

#include "logger.h"

#include <fstream>

class FileLogger : public LoggingObserver {
protected:
  ofstream of;

public:
  FileLogger(const string& path) {
    of.open(path.c_str(), std::ios_base::app);
  }
  ~FileLogger() {}

  void HandleLogEntry(const string& entry) {
    of << entry;
    of.flush();
  }
};

