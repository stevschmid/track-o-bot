#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>
#include <QSslError>

class Tracker : public QObject
{
  Q_OBJECT

DEFINE_SINGLETON( Tracker );

private:
  QNetworkAccessManager mNetworkManager;
  QSettings mSettings;

  int mSuccessfulResultCount;
  int mUnknownOutcomeCount;
  int mUnknownModeCount;
  int mUnknownOrderCount;
  int mUnknownClassCount;
  int mUnknownOpponentCount;

  QNetworkReply* AuthPostJson( const QString& path, const QByteArray& data );

private slots:
  void AddResultHandleReply();
  void CreateAndStoreAccountHandleReply();
  void OpenProfileHandleReply();

  void SSLErrors( QNetworkReply *reply, const QList<QSslError>& errors );

  QNetworkRequest CreateTrackerRequest( const QString& path );

public:
  bool IsAccountSetUp() const;

  void AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList, int durationInSeconds, int rank, int legend );
  void CreateAndStoreAccount();
  void OpenProfile();
  void EnsureAccountIsSetUp();

  QString Username() const;
  void SetUsername( const QString& username );

  QString Password() const;
  void SetPassword( const QString& password );

  QString WebserviceURL();
  QString WebserviceURL( const QString& path );
  void SetWebserviceURL( const QString& webserviceUrl );

signals:
  void AccountCreated();

};

