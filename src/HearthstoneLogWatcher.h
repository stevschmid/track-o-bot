#pragma once

#include <QString>

class HearthstoneLogWatcher : public QObject
{
  Q_OBJECT

private:
  QString mPath;
  qint64 mLastSeekPos;

public:
  HearthstoneLogWatcher( QObject *parent, const QString& path );

private slots:
  void CheckForLogChanges();

signals:
  void LineAdded( const QString& line );

};
