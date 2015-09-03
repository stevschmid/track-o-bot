#pragma once

#include <time.h>

#include <QFile>
#include <QString>

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
  Q_OBJECT

DEFINE_SINGLETON( Logger )

private:
  QFile *mFile;

public:
  void SetLogPath( const QString& path );
  void Add( LogEventType type, const char *fmt, ... );

signals:
  void NewMessage( LogEventType type, const QString& message );
};

