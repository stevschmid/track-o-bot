#pragma once

#include <time.h>

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <QDebug>

class Logger : public QObject {
  Q_OBJECT;

DEFINE_SINGLETON(Logger)

private:
  std::ofstream of;

public:
  void SetLogPath(const string& path) {
    of.open(path.c_str(), std::ios_base::app);
  }

  void Add(const char *fmt, ...) {
    char buffer[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    Add(string(buffer));
  }

  void Add(const string& message) {
    // Timestamp
    char timestamp[256];
    time_t t = time(0);
    struct tm *now = localtime(&t);
    strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", now);

    string line = string(timestamp) + message + "\n";

    // Add to file
    if(of.is_open()) {
      of << line;
      of.flush();
    }

    emit NewMessage(line);
  }

signals:
  void NewMessage(const string& message);
};

