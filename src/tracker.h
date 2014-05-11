#pragma once

#include <QNetworkAccessManager>
#include <QSettings>

class Tracker : public QObject
{
  Q_OBJECT

private:
  QNetworkAccessManager networkManager;
  QSettings settings;

  QString Username();
  QString Password();
  void SetUsername(const QString& username);
  void SetPassword(const QString& password);
  bool IsAccountSetUp();

private slots:
  void EnsureAccountIsSetUp();
  void AddResultHandleReply();
  void CreateAndStoreAccountHandleReply();
  void OpenProfileHandleReply();

public:
  Tracker();
  void AddResult(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass);
  void CreateAndStoreAccount();
  void OpenProfile();

};

