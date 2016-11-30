#include "HearthstoneCardDB.h"
#include "Settings.h"
#include "Hearthstone.h"

#include <QtXml>

HearthstoneCardDB::HearthstoneCardDB( QObject *parent )
  : QObject( parent )
{
}

int HearthstoneCardDB::Count() const {
  return mCards.count();
}

bool HearthstoneCardDB::Contains( const QString& id ) const {
  return mCards.contains( id );
}

int HearthstoneCardDB::Mana( const QString& id ) const {
  return mCards[ id ][ "mana" ].toInt();
}

QString HearthstoneCardDB::Name( const QString& id ) const {
  return mCards[ id ][ "name" ].toString();
}

QString HearthstoneCardDB::Type( const QString& id ) const {
  int value = mCards[ id ][ "type" ].toInt();
  switch( value ) {
    case 3:
    case 4:
      return "minion";
      break;

    case 5:
    case 6:
      return "ability";

    case 7:
      return "weapon";

    case 10:
      return "hero";

    default:
      return "unknown";
  }
}

QMap< QString, QVariantMap > ReadTags( const QString& locale ) {
  UNUSED_ARG( locale );

  QMap< QString, QVariantMap > tags;

  QString path = QString( "%1/DBF/CARD_TAG.xml" ).arg( Settings::Instance()->HearthstoneDirectoryPath() );
  QFile file( path );
  if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    DBG( "Couldn't open %s", qt2cstr( path ) );
    return tags;
  }

  QString xmlData = file.readAll();

  QDomDocument doc;
  doc.setContent( xmlData );

  QDomElement dbf = doc.firstChildElement( "Dbf" );
  QDomElement record = dbf.firstChildElement( "Record" );
  for( ; !record.isNull(); record = record.nextSiblingElement( "Record" ) ) {
    QVariantMap item;

    QDomElement field = record.firstChildElement( "Field" );
    for( ; !field.isNull(); field = field.nextSiblingElement( "Field" ) ) {
      QString key = field.attribute( "column" );
      QString value = field.text();
      item[ key ] = value;
    }

    if( item.contains( "CARD_ID" ) ) {
      QString cardId = item[ "CARD_ID" ].toString();

      QString tagKey = item[ "TAG_ID" ].toString();
      QString tagValue = item[ "TAG_VALUE" ].toString();

      tags[ cardId ][ tagKey ] = tagValue;
    }
  }

  return tags;
}

QMap< QString, QVariantMap > ReadCards( const QString& locale ) {
  QMap< QString, QVariantMap > cards;

  QString path = QString( "%1/DBF/CARD.xml" ).arg( Settings::Instance()->HearthstoneDirectoryPath() );
  QFile file( path );
  if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    DBG( "Couldn't open %s", qt2cstr( path ) );
    return cards;
  }

  QString xmlData = file.readAll();

  QDomDocument doc;
  doc.setContent( xmlData );

  QDomElement dbf = doc.firstChildElement( "Dbf" );
  QDomElement record = dbf.firstChildElement( "Record" );
  for( ; !record.isNull(); record = record.nextSiblingElement( "Record" ) ) {
    QVariantMap card;

    QDomElement field = record.firstChildElement( "Field" );
    for( ; !field.isNull(); field = field.nextSiblingElement( "Field" ) ) {
      QString key = field.attribute( "column" );

      QString value;
      QDomElement localizedName = field.firstChildElement( locale );
      QDomElement defaultLocalizedName = field.firstChildElement( "enUS" );
      if( !locale.isEmpty() && !localizedName.isNull() ) {
        value = localizedName.text();
      } else if( !defaultLocalizedName.isNull() ) {
        value = defaultLocalizedName.text();
      } else {
        value = field.text();
      }

      card[ key ] = value;
    }

    if( card.contains( "ID" ) ) {
      cards[ card[ "ID" ].toString() ] = card;
    }
  }

  return cards;
}

bool HearthstoneCardDB::Load() {
  Unload();

  DBG( "Load Card DB" );

  QString locale = Locale();

  QMap< QString, QVariantMap > cards = ReadCards( locale );
  QMap< QString, QVariantMap > tags = ReadTags( locale );

  for( auto key : cards.keys() ) {
    QVariantMap cardRaw = cards[ key ];
    QVariantMap cardTags = tags[ key ];

    QVariantMap card;

    QString ref = cardRaw[ "NOTE_MINI_GUID" ].toString();
    card[ "name" ] = cardRaw[ "NAME" ].toString();
    card[ "mana" ] = cardTags[ "48" ].toString();
    card[ "type" ] = cardTags[ "202" ].toString();
    mCards[ ref ] = card;

    DBG( "%s Name %s Cost %d Type %s", qt2cstr( ref ), qt2cstr( Name( ref ) ), Mana( ref ), qt2cstr( Type( ref ) ) );
  }

  return true;
}

bool HearthstoneCardDB::Unload() {
  DBG( "Unload Card DB" );
  mCards.clear();
  return true;
}

bool HearthstoneCardDB::Loaded() const {
  return !mCards.empty();
}

QString HearthstoneCardDB::Locale() const {
  QString locale = "enUS";

  QString path = QString( "%1/Launcher.db" ).arg( Settings::Instance()->HearthstoneDirectoryPath() );
  QFile file( path );

  if( file.open( QIODevice::ReadOnly ) )  {
    QTextStream stream( &file );
    locale = stream.readLine();
  } else {
    DBG( "Couldn't open %s to determine locale", qt2cstr( path ) );
  }

  DBG( "Locale = %s", qt2cstr( locale )) ;
  return locale;
}

