#pragma once

#include <QNetworkAccessManager>
#include <QSettings>

class Tracker : public QObject
{
  Q_OBJECT

DEFINE_SINGLETON(Tracker)

private:
  QNetworkAccessManager networkManager;
  QSettings settings;

  bool IsAccountSetUp();

private slots:
  void AddResultHandleReply();
  void CreateAndStoreAccountHandleReply();
  void OpenProfileHandleReply();

public:
  void AddResult(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass);
  void CreateAndStoreAccount();
  void OpenProfile();
  void EnsureAccountIsSetUp();

  QString Username();
  void SetUsername(const QString& username);

  QString Password();
  void SetPassword(const QString& password);

  QString WebserviceURL();
  void SetWebserviceURL(const QString& webserviceUrl);

};

