#pragma once

#include <time.h>

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <QDebug>

class Logger : public QObject {
  Q_OBJECT;

DEFINE_SINGLETON( Logger )

private:
  std::ofstream mOf;

public:
  void SetLogPath( const string& path );
  void Add( const char *fmt, ... );
  void Add( const string& message );

signals:
  void NewMessage( const string& message );
};

