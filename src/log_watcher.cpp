#include "log_watcher.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>

LogWatcher::LogWatcher(const string& logPath)
  :path(logPath)
{
  watcher.addPath(logPath.c_str());
  connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(LogChanged(QString)));

  QFile file(logPath.c_str());
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
      if(!line.trimmed().isEmpty()) {

        QRegExp regex("ProcessChanges.*cardId=(\\w+).*zone from (.*) -> (.*)");
        if(regex.indexIn(line) != -1) {
          QStringList list = regex.capturedTexts();
          QString cardId = list[1];
          QString from = list[2];
          QString to = list[3];
          LOG("Card %s %s %s", cardId.toStdString().c_str(), from.toStdString().c_str(), to.toStdString().c_str());
        }
      }
    }

    lastSeekPos = stream.pos();
  }
}

