#include "logger.h"

void Logger::Add(const string& message) {
  Notify(message);
}

void Logger::RegisterObserver(LoggingObserver *observer) {
  observers.push_back(observer);
}

void Logger::UnregisterObserver(LoggingObserver *observer) {
  observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
}

void Logger::Notify(const string& message) {
  vector<LoggingObserver*>::iterator it;
  for(it = observers.begin(); it != observers.end(); ++it) {
    (*it)->HandleLogEntry(message);
  }
}
