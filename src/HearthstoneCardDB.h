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

class CardCollector : public QObject, public QXmlDefaultHandler
{
  Q_OBJECT

private:
  QString mNextAttributeName;
  QString mCurrentCardId;
  QVariantMap mCurrentAttributes;

public:
  CardCollector()
    : QXmlDefaultHandler(), mNextAttributeName()
  {
  }

  ~CardCollector() {
  }

  bool startElement( const QString&,
      const QString&,
      const QString& qName,
      const QXmlAttributes& attribs )
  {
    static const QMap< int, QString > ATTRIBUTES_BY_TAG_ID = {
      { 185, "name" },
      { 48, "cost" },
      { 202, "type" }
    };

    if( qName == "Entity" ) {
      mCurrentCardId = attribs.value( "CardID" );
    } else if( qName == "Tag" ) {

      int enumId = attribs.value( "enumID" ).toInt();
      if( ATTRIBUTES_BY_TAG_ID.contains( enumId ) ) {
        const QString& attributeName = ATTRIBUTES_BY_TAG_ID[ enumId ];

        mNextAttributeName = "";
        QString value = attribs.value( "value" );
        if( value.isEmpty() ) {
          // read <tag>value</tag>
          mNextAttributeName = attributeName;
        } else {
          // read <tag value="1"></tag>
          mCurrentAttributes[ attributeName ] = value;
        }
      }

    }

    return true;
  }

  bool characters( const QString& ch ) {
    if( !mNextAttributeName.isEmpty() ) {
      mCurrentAttributes[ mNextAttributeName ] = ch;
      mNextAttributeName = "";
    }
    return true;
  }

  bool endElement( const QString&,
      const QString&,
      const QString& qName )
  {
    if( qName == "Entity" && !mCurrentCardId.isEmpty() ) {
      emit CardCollected( mCurrentCardId, mCurrentAttributes );

      mCurrentCardId = "";
      mCurrentAttributes.clear();
    }

    return true;
  }

signals:
  void CardCollected( const QString& id, const QVariantMap& card );

};

