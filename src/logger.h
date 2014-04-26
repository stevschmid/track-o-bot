#pragma once

#include <vector>
#include <sstream>

class LoggingObserver {
public:
  virtual ~LoggingObserver() { }
  virtual void HandleLogEntry(const string& message) = 0;
};

class Logger {
protected:
  vector<LoggingObserver*> observers;
  void Notify(const string& message);

public:
  void RegisterObserver(LoggingObserver *observer);
  void UnregisterObserver(LoggingObserver *observer);
  void Add(const string& message);

  template <typename T>
  friend Logger& operator<<(Logger& logger, T thing) {
    stringstream ss;
    ss << thing;
    logger.Add(ss.str());
    return logger;
  }

  friend Logger& operator<<(Logger& logger, ostream& (*pf) (ostream&))
  {
    stringstream ss;
    ss << *pf;
    logger.Add(ss.str());
    return logger;
  }
};

