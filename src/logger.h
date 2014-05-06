#pragma once

#include <vector>
#include <sstream>
#include <fstream>

#include <QDebug>

class Logger {
DEFINE_SINGLETON(Logger)

protected:
  ofstream of;

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
    strftime(timestamp, sizeof(timestamp), "[%T] ", now);

    string line = string(timestamp) + message;

    // Add to file
    if(of.is_open()) {
      of << line << endl;
      of.flush();
    }
    // Add to QT
    qDebug() << qPrintable(QString::fromStdString(line)) << endl;
  }
};

