#pragma once

class HearthstoneLogWatcher : public QObject
{
  Q_OBJECT

private:
  QString mPath;
  qint64 mLastSeekPos;

public:
  HearthstoneLogWatcher();

private slots:
  void CheckForLogChanges();

signals:
  void LineAdded( const QString& line );

};
