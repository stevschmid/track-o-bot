#include "log_watcher.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>

LogWatcher::LogWatcher(const QString& logPath)
  :path(logPath)
{
  watcher.addPath(logPath);
  connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(LogChanged(QString)));

  QFile file(logPath);
  lastSeekPos = file.size();
  LOG("File size %d", lastSeekPos);
}

void LogWatcher::LogChanged(const QString& changedFilePath) {
  QFile file(changedFilePath);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    LOG("Couldn't open %s", changedFilePath.toStdString().c_str());
  }

  qint64 size = file.size();
  if(size < lastSeekPos) {
    lastSeekPos = size;
  } else {
    QTextStream stream(&file);
    stream.seek(lastSeekPos);

    while(!stream.atEnd()) {
      QString line = stream.readLine();
      emit LineAdded(line);
    }

    lastSeekPos = stream.pos();
  }
}

