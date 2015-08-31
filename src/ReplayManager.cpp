#include "ReplayManager.h"
#include "Hearthstone.h"

#include <QDir>
#include <QTimer>

#define REPLAY_WIDTH   1280
#define REPLAY_HEIGHT  800
#define REPLAY_BITRATE 500
#define REPLAY_FPS     1

ReplayManager::ReplayManager( HearthstoneLogTracker *logTracker )
  : mSpectating( false ),
    mDropboxPath( RetrieveDropboxPath() ),
    mWriter(
      REPLAY_WIDTH, REPLAY_HEIGHT,
      REPLAY_BITRATE, REPLAY_FPS
    )
{
  connect( logTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( logTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&) ), this, SLOT( HandleMatchEnd(const ::CardHistoryList&) ) );

  connect( logTracker, SIGNAL( HandleTurn(int) ), this, SLOT( HandleTurn(int) ) );
  connect( logTracker, SIGNAL( HandleSpectating(bool) ), this, SLOT( HandleSpectating(bool) ) );

  connect( Tracker::Instance(), SIGNAL( UploadResultSucceeded(const QJsonObject&) ), this, SLOT( UploadResultSucceeded(const QJsonObject&) ) );
}

ReplayManager::~ReplayManager() {
  if( mWriter.IsOpen() ) {
    mWriter.Close();
  }
}

void ReplayManager::HandleMatchStart() {
  if( mWriter.IsOpen() ) {
    mWriter.Close();
  }

  QString path = AppFile( "Temp.webm" );
  if( !mWriter.Open( path ) ) {
    ERR( "Couldn't open battleboard writer %s\n", qt2cstr( path ) );
  }
}

void ReplayManager::HandleTurn( int turnCounter ) {
  UNUSED_ARG( turnCounter );

  if( !mSpectating ) {
    QTimer::singleShot( 3500, this, [=]() {
      QPixmap screen;

      if( Hearthstone::Instance()->CaptureWholeScreen( &screen ) ) {
        mWriter.AddFrame( screen.toImage() );
      } else {
        ERR( "Couldn't capture battleboard" );
      }
    });
  }
}

void ReplayManager::HandleSpectating( bool nowSpectating ) {
  mSpectating = nowSpectating;
}

void ReplayManager::UploadResultSucceeded( const QJsonObject& response ) {
  int id = response[ "result" ].toObject()[ "id" ].toInt();
  if( !id ) {
    ERR( "Invalid ID received. Cannot save battleboards" );
    return;
  }

  mWriter.Close();

  QString src = AppFile( "Temp.webm" );
  QString dst = AppFile( QString( "%1.webm" ).arg( id ) );
  if( !QFile::rename( src, dst ) ) {
    ERR( "Couldn't move battleboards" );
  }
}

QString ReplayManager::AppFile( const QString& fileName ) const {
  QString path = mDropboxPath + "/Apps/Track-o-Bot/";
  if( !QDir( path ).exists() && !QDir().mkpath( path ) ) {
    ERR( "Couldn't create path %s", qt2cstr( path ) );
  }

  return path + fileName;
}

QString ReplayManager::RetrieveDropboxPath() {
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
