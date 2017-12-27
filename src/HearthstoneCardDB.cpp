#include "HearthstoneCardDB.h"
#include "Settings.h"
#include "Hearthstone.h"

#include <QtXml>
#include <cassert>

#define HEARTHSTONE_JSON_API_URL "https://api.hearthstonejson.com/v1"

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

int HearthstoneCardDB::Cost( const QString& id ) const {
  return mCards[ id ][ "cost" ].toInt();
}

QString HearthstoneCardDB::Name( const QString& id ) const {
  return mCards[ id ][ "name" ].toString();
}

QString HearthstoneCardDB::Type( const QString& id ) const {
  return mCards[ id ][ "type" ].toString();
}

QString HearthstoneCardDB::CardsJsonLocalPath() {
  int build = Hearthstone::Instance()->Build();
  QString locale = Hearthstone::Instance()->DetectLocale();
  QString appDataLocation = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).first();
  return QString( "%1/cards_%2_%3.json" ).arg( appDataLocation ).arg( build ).arg( locale );
}

QString HearthstoneCardDB::CardsJsonRemoteUrl() {
  int build = Hearthstone::Instance()->Build();
  QString locale = Hearthstone::Instance()->DetectLocale();
  return QString( "%1/%2/%3/cards.json" ).arg( HEARTHSTONE_JSON_API_URL ).arg( build ).arg( locale );
}

bool HearthstoneCardDB::Load() {
  // Request cards.json if needed only once
  static bool cardsRequested = false;

  Unload();

  int build = Hearthstone::Instance()->Build();
  if( !build ) {
    ERR( "Could not determine build during card db load" );
    return false;
  }

  if( QFileInfo( CardsJsonLocalPath() ).exists() ) {
    DBG( "cards.json already downloaded, load it locally: %s", qt2cstr( CardsJsonLocalPath() ) );
    LoadJson();
  } else {
    if( !cardsRequested ) {
      cardsRequested = true;

      DBG( "cards.json not downloaded or outdated, download it: %s", qt2cstr( CardsJsonLocalPath() ) );
      DBG( "Download cards.json from: %s", qt2cstr( CardsJsonRemoteUrl() ) );

      QNetworkRequest request( CardsJsonRemoteUrl() );
      QNetworkReply *reply = mNetworkManager.get( request );
      connect( reply, &QNetworkReply::finished, this, &HearthstoneCardDB::CardsJsonReply );
    }
  }

  return false;
}

void HearthstoneCardDB::CardsJsonReply() {
  QNetworkReply *reply = static_cast< QNetworkReply* >( sender() );
  QByteArray jsonData = reply->readAll();

  DBG( "Downloaded cards.json %d bytes", jsonData.size() );

  QString dirPath = QFileInfo( CardsJsonLocalPath() ).absolutePath();
  if( !QFile::exists( dirPath ) ) {
    QDir dir;
    dir.mkpath( dirPath );
  }

  QFile file( CardsJsonLocalPath() );
  file.open( QIODevice::WriteOnly );
  file.write( jsonData );
  file.close();

  LoadJson();
}

void HearthstoneCardDB::LoadJson() {
  DBG( "LoadJson %s", qt2cstr( CardsJsonLocalPath() ) );

  if( !QFileInfo( CardsJsonLocalPath() ).exists() ) {
    DBG( "cards.json does not exist locally", qt2cstr( CardsJsonLocalPath() ) );
    return;
  }

  QFile file( CardsJsonLocalPath() );
  bool opened = file.open( QIODevice::ReadOnly | QIODevice::Text );
  assert( opened );

  QByteArray jsonData = file.readAll();
  QJsonParseError error;
  QJsonArray jsonCards = QJsonDocument::fromJson( jsonData, &error ).array();
  assert( error.error == QJsonParseError::NoError );

  DBG( "Load cards.json" );

  for( QJsonValueRef jsonCardRef : jsonCards ) {
    QJsonObject jsonCard = jsonCardRef.toObject();

    QString ref = jsonCard[ "id" ].toString();

    QVariantMap card;
    card[ "name" ] = jsonCard[ "name" ].toString();
    card[ "cost" ] = jsonCard[ "cost" ].toInt();
    card[ "type" ] = jsonCard[ "type" ].toString();
    mCards[ ref ] = card;
    /* DBG( "%s Name %s Cost %d Type %s", qt2cstr( ref ), qt2cstr( Name( ref ) ), Cost( ref ), qt2cstr( Type( ref ) ) ); */
  }

  DBG( "Card DB %d cards", mCards.count() );
}

bool HearthstoneCardDB::Unload() {
  DBG( "Unload Card DB" );
  mCards.clear();
  return true;
}

bool HearthstoneCardDB::Loaded() const {
  return !mCards.empty();
}

