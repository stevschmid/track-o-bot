#include "BattleBoard.h"
#include "Hearthstone.h"

#include <QDir>

BattleBoard::BattleBoard( HearthstoneLogTracker *logTracker )
  : mSpectating( false )
{
  connect( logTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( logTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&) ), this, SLOT( HandleMatchEnd(const ::CardHistoryList&) ) );

  connect( logTracker, SIGNAL( HandleTurn(int) ), this, SLOT( HandleTurn(int) ) );
  connect( logTracker, SIGNAL( HandleSpectating(bool) ), this, SLOT( HandleSpectating(bool) ) );

  connect( Tracker::Instance(), SIGNAL( UploadResultSucceeded(const QJsonObject&) ), this, SLOT( UploadResultSucceeded(const QJsonObject&) ) );
}

void BattleBoard::HandleMatchStart() {
  mScreenshots.clear();
}


void BattleBoard::HandleTurn( int turnCounter ) {
  if( !mSpectating ) {
    QPixmap screen;

    if( Hearthstone::Instance()->CaptureWholeScreen( &screen ) ) {
      QString name = QString("%1").arg( turnCounter );
      mScreenshots[ name ] = screen;

      DBG( "BattleBoard Screen %s", name.toStdString().c_str() );
    }
  }
}

void BattleBoard::HandleSpectating( bool nowSpectating ) {
  mSpectating = nowSpectating;
}

void BattleBoard::UploadResultSucceeded( const QJsonObject& response ) {
  int id = response[ "result" ].toObject()[ "id" ].toInt();
  if( !id ) {
    ERR( "Invalid ID received. Cannot save battleboards" );
    return;
  }

  QString directoryPath = QString("/Users/spidy/Dropbox/Apps/Track-o-Bot/%1").arg( id );
  if( !QDir( directoryPath ).exists() && !QDir().mkdir( directoryPath ) ) {
    ERR( "Couldn't create path %s", directoryPath.toStdString().c_str() );
    return;
  }

  QMapIterator <QString, QPixmap> it( mScreenshots );
  while( it.hasNext() ) {
    it.next();

    const QString& name = it.key();
    const QPixmap& screenshot = it.value();

    QString path = QDir( directoryPath ).filePath( QString("%1.png").arg( name ) );
    DBG( "Save BattleBoard Screen %s", path.toStdString().c_str() );
    screenshot.save( path );
  }

}
