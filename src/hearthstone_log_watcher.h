#pragma once

#define CHECK_FOR_LOG_CHANGES_INTERVAL_MS 500

class HearthstoneLogWatcher : public QObject
{
  Q_OBJECT

private:
  QString path;
  qint64 lastSeekPos;

public:
  HearthstoneLogWatcher();

private slots:
  void CheckForLogChanges();

signals:
  void LineAdded(const QString& line);

};
