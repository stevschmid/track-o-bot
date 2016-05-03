#include "HearthstoneCardDB.h"
#include "Settings.h"
#include "Hearthstone.h"

#include <QtXml>

QString ExtractCardDefsXmlFromUnity3d( QString fileName, QString desiredLocale ) {
  QFile file( fileName );

  if( !file.open( QIODevice::ReadOnly ) ) {
    DBG( "Couldn't open %s", qt2cstr( fileName ) );
    return "";
  }

  QString xml;

  QByteArray data = file.readAll();
  for( int idx = data.indexOf( "<CardDefs>" ); idx != -1; idx = data.indexOf( "<CardDefs>", idx + 1 ) ) {
    // Determine size
    int size;
    memcpy( &size, data.constData() + ( idx - 4 ), 4 );

    // Determine locale
    int localeOffset = idx - 5;
    while( localeOffset > 0 && data[ localeOffset - 1 ] != char(0) )
      localeOffset--;

    int localeSizeOffset = localeOffset - 4;
    if( localeOffset < 0 && localeSizeOffset < 0 ) {
      DBG( "Offsets < 0. Skip" );
      continue;
    }

    int localeSize;
    memcpy( &localeSize, data.constData() + localeSizeOffset, 4 );
    QString locale = QString::fromUtf8( data.constData() + localeOffset, localeSize );

    if( locale == desiredLocale ) {
      DBG( "Found desired locale %s (card defs size %d)", qt2cstr( desiredLocale ), size );
      xml = QString::fromUtf8( data.constData() + idx, size );
      break;
    }
  }

  return xml;
}

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
  return mCards[ id ][ "cost" ].toInt();
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

bool HearthstoneCardDB::Load() {
  Unload();

  DBG( "Load Card DB" );

  QString locale = Locale();
  QString platform = "Win";
#ifdef Q_OS_MAC
  platform = "OSX";
#endif

  QString path = QString( "%1/Data/%2/cardxml0.unity3d" ).arg( Settings::Instance()->HearthstoneDirectoryPath() ).arg( platform );
  QString xmlData = ExtractCardDefsXmlFromUnity3d( path, locale );

  if( !xmlData.isEmpty() ) {
    QXmlSimpleReader reader;
    CardCollector cardCollector;
    reader.setContentHandler( &cardCollector );

    connect( &cardCollector, &CardCollector::CardCollected, this, [&]( const QString& id, const QVariantMap& card ) {
      mCards[ id ] = card;
    });

    QXmlInputSource input;
    input.setData( xmlData );
    reader.parse( &input );

    DBG( "Loaded %d cards", mCards.count() );
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

