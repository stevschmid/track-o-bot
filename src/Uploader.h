#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>
#include <QSslError>
#include <QJsonObject>

class Uploader : public QObject
{
  Q_OBJECT

DEFINE_SINGLETON( Uploader )

private:
  QNetworkAccessManager mNetworkManager;

  QNetworkReply* AuthPostJson( const QString& path, const QByteArray& data );

private slots:
  void CreateAndStoreAccountHandleReply();
  void OpenProfile();
  void OpenProfileHandleReply();

  void SSLErrors( QNetworkReply *reply, const QList<QSslError>& errors );

  QNetworkRequest CreateUploaderRequest( const QString& path );

public:
  bool IsAccountSetUp() const;

  void UploadResult( const QJsonObject& result );
  void CreateAndStoreAccount();
  void EnsureAccountIsSetUp();

  QString WebserviceURL( const QString& path );

signals:
  void UploadResultFailed( const QJsonObject& result, int errorCode );
  void UploadResultSucceeded( const QJsonObject& result );

};

