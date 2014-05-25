#pragma once

#include <QFileSystemWatcher>

class LogWatcher : public QObject
{
  Q_OBJECT

private:
  QString path;
  QFileSystemWatcher watcher;
  qint64 lastSeekPos;

public:
  LogWatcher(const QString& logPath);

private slots:
  void LogChanged(const QString& changedFilePath);

signals:
  void LineAdded(const QString& line);

};
