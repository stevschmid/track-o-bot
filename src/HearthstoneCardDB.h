#pragma once

#include <QMap>
#include <QString>
#include <QVariant>

#include <QtXml>

class HearthstoneCardDB : public QObject
{
  Q_OBJECT

private:
  QMap< QString, QVariantMap > mCards;

private:
  QString Locale() const;

public:
  HearthstoneCardDB( QObject *parent = 0 );

  bool Load();
  bool Unload();

  bool Loaded() const;

  int Count() const;
  bool Contains( const QString& id ) const;

  int Mana( const QString& id ) const;
  QString Name( const QString& id ) const;
  QString Type( const QString& id ) const;
};

