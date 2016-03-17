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
  QTimer*     mUploadTimer;
  QJsonArray  mQueue;
  WebProfile  mWebProfile;

  void Load();
  void Save();

  void UploadResult( const QJsonObject& result );

private slots:
  void UploadResultFailed( const QJsonObject& result, int replyCode, int httpStatusCode );
  void UploadResultSucceeded( const QJsonObject& response );

  void UploadQueue();

signals:
  void ResultUploaded( int id );

public:
  ResultQueue();
  ~ResultQueue();

  void Add( const Result& result );
};
