#pragma once

#include <QMap>
#include <QString>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QtXml>

class HearthstoneCardDB : public QObject
{
  Q_OBJECT

private:
  QMap< QString, QVariantMap > mCards;

  QNetworkAccessManager mNetworkManager;
  void CardsJsonReply();

  QString CardsJsonLocalPath();
  QString CardsJsonRemoteUrl();

private:
  void LoadJson();

public:
  HearthstoneCardDB( QObject *parent = 0 );

  bool Load();
  bool Unload();

  bool Loaded() const;

  int Count() const;
  bool Contains( const QString& id ) const;

  int Cost( const QString& id ) const;
  QString Name( const QString& id ) const;
  QString Type( const QString& id ) const;
};

