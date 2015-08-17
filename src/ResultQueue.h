#pragma once

#include "Tracker.h"
#include "Result.h"

#include <QTimer>
#include <QSettings>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class ResultQueue : public QObject
{
  Q_OBJECT

private:
  QTimer*    mCheckTimer;
  QTimer*    mUploadTimer;
  QJsonArray mQueue;

  void Load();
  void Save();

private slots:
  void UploadResultFailed( const QJsonObject& result, int errorCode );
  void UploadResultSucceeded( const QJsonObject& response );

  void Check();
  void Upload();

public:
  ResultQueue();
  ~ResultQueue();

  void Add( const Result& result );
};
