#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>
#include <QSslError>
#include <QJsonObject>

class WebProfile : public QObject
{
  Q_OBJECT

private:
  QNetworkAccessManager mNetworkManager;

  QNetworkReply* AuthPostJson( const QString& path, const QByteArray& data );

public slots:
  void OpenProfile();

private slots:
  void CreateAndStoreAccountHandleReply();
  void OpenProfileHandleReply();

  void SSLErrors( QNetworkReply *reply, const QList<QSslError>& errors );

  QNetworkRequest CreateWebProfileRequest( const QString& path );

signals:
  void UploadResultFailed( const QJsonObject& result, int errorCode );
  void UploadResultSucceeded( const QJsonObject& response );

public:
  WebProfile();

  bool IsAccountSetUp() const;
  void UploadResult( const QJsonObject& result );
  void CreateAndStoreAccount();
  void EnsureAccountIsSetUp();

  QString WebserviceURL( const QString& path );
};

