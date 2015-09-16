#pragma once

#include "Result.h"
#include "WebProfile.h"

#include <QTimer>
#include <QSettings>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class ResultQueue : public QObject
{
  Q_OBJECT

private:
  QTimer*     mCheckTimer;
  QTimer*     mUploadTimer;
  QJsonArray  mQueue;
  WebProfile  mWebProfile;

  void Load();
  void Save();

private slots:
  void UploadResultFailed( const QJsonObject& result, int errorCode );
  void UploadResultSucceeded( const QJsonObject& response );

  void Check();
  void Upload();

signals:
  void ResultUploaded( int id );

public:
  ResultQueue();
  ~ResultQueue();

  void Add( const Result& result );
};
