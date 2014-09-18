#pragma once
#include "Local.h"
#include <QDebug>

#define CHECK_FOR_LOG_CHANGES_INTERVAL_MS 500

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
