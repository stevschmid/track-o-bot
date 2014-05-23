#pragma once

#include <QFileSystemWatcher>

class LogWatcher : public QObject
{
  Q_OBJECT

private:
  string path;
  QFileSystemWatcher watcher;
  qint64 lastSeekPos;

public:
  LogWatcher(const string& logPath);

private slots:
  void LogChanged(const QString& changedFilePath);

};

