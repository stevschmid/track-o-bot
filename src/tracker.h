#pragma once

#include <QNetworkAccessManager>

class Tracker : public QObject
{
  Q_OBJECT

protected:
  QNetworkAccessManager networkManager;

private slots:
  void OnNetworkReply(QNetworkReply *reply);

public:
  Tracker();
  void AddResult(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass);
};

