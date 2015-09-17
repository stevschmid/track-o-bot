#include "HearthstoneLogWatcher.h"
#include "Hearthstone.h"

#include <QFile>
#include <QTimer>

#define CHECK_FOR_LOG_CHANGES_INTERVAL_MS 50

#include <QTextStream>
HearthstoneLogWatcher::HearthstoneLogWatcher( QObject *parent, const QString& path )
  : QObject( parent ),
    mPath( path ),
    mLastSeekPos( 0 )
{
  // We used QFileSystemWatcher before but it fails on windows
  // Windows File Notification seems to be very tricky with files
  // which are not explicitly flushed (which is the case for the Hearthstone Log)
  // QFileSystemWatcher fails, manual implemention with FindFirstChangeNotification
  // fails. So instead of putting too much work into a file-system depending solution
  // just use a low-overhead polling strategy
  QTimer *timer = new QTimer( this );
  connect( timer, SIGNAL( timeout() ), this, SLOT( CheckForLogChanges() ) );
  timer->start( CHECK_FOR_LOG_CHANGES_INTERVAL_MS );

  DBG( "Watch log %s",  qt2cstr( mPath ) );

  QFile file( mPath );
  if( file.exists() ) {
    mLastSeekPos = file.size();
  }
}

void HearthstoneLogWatcher::CheckForLogChanges() {
  // Only access disk when HS is running
  if( !Hearthstone::Instance()->Running() ) {
    return;
  }

  QFile file( mPath );
  if( !file.open( QIODevice::ReadOnly ) ) {
    return;
  }

  qint64 size = file.size();
  if( size < mLastSeekPos ) {
    DBG( "Log truncation detected. This is OK if game was restarted." );
    mLastSeekPos = 0;
  } else {
    // Use raw QFile instead of QTextStream
    // QTextStream uses buffering and seems to skip some lines (see also QTextStream#pos)
    file.seek( mLastSeekPos );

    QByteArray buf = file.readAll();
    QList< QByteArray > lines = buf.split('\n');

    QByteArray lastLine = lines.takeLast();
    for( const QByteArray& line : lines ) {
      emit LineAdded( QString::fromUtf8( line.trimmed() ) );
    }

    mLastSeekPos = file.pos() - lastLine.size();
  }
}

