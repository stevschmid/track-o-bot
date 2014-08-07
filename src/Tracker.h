#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSettings>
#include <QSslError>

class Tracker : public QObject
{
  Q_OBJECT

DEFINE_SINGLETON( Tracker )

private:
  QNetworkAccessManager networkManager;
  QSettings settings;

  QNetworkReply* AuthPostJson( const QString& path, const QByteArray& data );

  int successfulResultCount;
  int unknownOutcomeCount;
  int unknownModeCount;
  int unknownOrderCount;
  int unknownClassCount;
  int unknownOpponentCount;

private slots:
  void AddResultHandleReply();
  void CreateAndStoreAccountHandleReply();
  void OpenProfileHandleReply();

  void SSLErrors( QNetworkReply *reply, const QList<QSslError>& errors );

  QNetworkRequest CreateTrackerRequest( const QString& path );

signals:
  void AccountCreated();

public:
  bool IsAccountSetUp();

  void AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList );
  void CreateAndStoreAccount();
  void OpenProfile();
  void EnsureAccountIsSetUp();

  QString Username();
  void SetUsername( const QString& username );

  QString Password();
  void SetPassword( const QString& password );

  QString WebserviceURL();
  QString WebserviceURL( const QString& path );
  void SetWebserviceURL( const QString& webserviceUrl );

};

