#include "Battleboard.h"
#include "Hearthstone.h"

#include <QDir>
#include <QTimer>

Battleboard::Battleboard( HearthstoneLogTracker *logTracker )
  : mSpectating( false ), mDropboxPath( RetrieveDropboxPath() )
{
  connect( logTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( logTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&) ), this, SLOT( HandleMatchEnd(const ::CardHistoryList&) ) );

  connect( logTracker, SIGNAL( HandleTurn(int) ), this, SLOT( HandleTurn(int) ) );
  connect( logTracker, SIGNAL( HandleSpectating(bool) ), this, SLOT( HandleSpectating(bool) ) );

  connect( Tracker::Instance(), SIGNAL( UploadResultSucceeded(const QJsonObject&) ), this, SLOT( UploadResultSucceeded(const QJsonObject&) ) );
}

void Battleboard::HandleMatchStart() {
  ClearScreenshots();
}

void Battleboard::HandleTurn( int turnCounter ) {
  if( !mSpectating ) {
    QTimer::singleShot( 3500, this, [=]() {
      QPixmap screen;

      if( Hearthstone::Instance()->CaptureWholeScreen( &screen ) ) {
        QString name = QString("%1.jpg").arg( turnCounter );
        SaveScreenshot( name, screen );

        DBG( "Battleboard Screen %s", name.toStdString().c_str() );
      }
    });
  }
}

void Battleboard::HandleSpectating( bool nowSpectating ) {
  mSpectating = nowSpectating;
}

void Battleboard::UploadResultSucceeded( const QJsonObject& response ) {
  int id = response[ "result" ].toObject()[ "id" ].toInt();
  if( !id ) {
    ERR( "Invalid ID received. Cannot save battleboards" );
    return;
  }

  QString path = QString( "%1" ).arg( id );
  MoveScreenshots( path );
}

void Battleboard::SaveScreenshot( const QString& name, const QPixmap& screenshot ) {
  QString path = AppDirectory( "Temp" );
  if( path.isEmpty() )
    return;

  if( !EnsureDirectory( path ) )
    return;

  QString filePath = path + "/" + name;
  DBG( "Save screenshot to %s", filePath.toStdString().c_str() );
  screenshot.save( filePath );
}

void Battleboard::ClearScreenshots() {
  QString path = AppDirectory( "Temp" );
  if( path.isEmpty() )
    return;

  QDir dir( path );
  if( !dir.exists() )
    return;

  dir.setFilter( QDir::NoDotAndDotDot | QDir::Files );
  for( QString dirFile : dir.entryList() ) {
    dir.remove( dirFile );
  }
}

bool Battleboard::MoveScreenshots( const QString& destination ) {
  QString src = AppDirectory( "Temp" );
  if( src.isEmpty() ) {
    return false;
  }

  QString dst = AppDirectory( destination );
  return QDir().rename( src, dst );
}

QString Battleboard::AppDirectory( const QString& subFolder ) const {
  return mDropboxPath + "/Apps/Track-o-Bot/" + subFolder;
}

bool Battleboard::EnsureDirectory( const QString& path ) {
  if( !QDir( path ).exists() && !QDir().mkpath( path ) ) {
    ERR( "Couldn't create path %s", path.toStdString().c_str() );
    return false;
  }
  return true;
}

QString Battleboard::RetrieveDropboxPath() {
#ifdef Q_OS_MAC
  QString path = QDir::homePath() + "/.dropbox/host.db";
#else
  QString path = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).first() + "/Dropbox/host.db";
#endif

  QFile file( path );
  if( !file.open( QFile::ReadOnly | QFile::Text ) )
    return QString();

  QString hostDbContents;

  QTextStream in( &file );
  while( !in.atEnd() ) {
    QString line = in.readLine();
    if( !line.isEmpty() ) {
      hostDbContents = line;
    }
  }

  if( hostDbContents.isEmpty() )
    return QString();

  QByteArray bytes;
  bytes.append( hostDbContents );
  return QByteArray::fromBase64( bytes );
}
