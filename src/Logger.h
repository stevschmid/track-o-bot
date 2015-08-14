#pragma once

#include <time.h>

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <QDebug>

typedef enum {
  LOG_DEBUG = 0,
  LOG_INFO = 1,
  LOG_ERROR = 2
} LogEventType;

const char LOG_EVENT_TYPE_NAMES[][128] = {
  "DEBUG",
  "INFO",
  "ERROR"
};

class Logger : public QObject {
  Q_OBJECT;

DEFINE_SINGLETON( Logger )

private:
  std::ofstream mOf;


public:
  void SetLogPath( const string& path );
  void Add( LogEventType type, const char *fmt, ... );

signals:
  void NewMessage( LogEventType type, const string& message );
};

