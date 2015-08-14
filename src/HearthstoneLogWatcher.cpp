#include "HearthstoneLogWatcher.h"
#include "Hearthstone.h"

#include <QFile>
#include <QTimer>

HearthstoneLogWatcher::HearthstoneLogWatcher()
  : mPath( Hearthstone::Instance()->LogPath() ), mLastSeekPos( 0 )
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
  if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    return;
  }

  qint64 size = file.size();
  if( size < mLastSeekPos ) {
    mLastSeekPos = size;
  } else {
    // Use raw QFile instead of QTextStream
    // QTextStream uses buffering and seems to skip some lines (see also QTextStream#pos)
    file.seek( mLastSeekPos );

    char c;
    while( !file.atEnd() ) {
      QString line = file.readLine();

      // We are not interested in the last line (in case it's not complete yet)
      if( file.atEnd() )
        break;

      // Make absolutely sure this line has a newline at the end
      file.seek( file.pos() - 1 );
      file.getChar( &c );
      if( c != 10 && c != 13 )
        break;

      emit LineAdded(line);
      mLastSeekPos = file.pos();
    }
  }
}



