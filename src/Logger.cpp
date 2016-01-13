#include "Logger.h"
#include <ctime>

#include <QTextStream>

DEFINE_SINGLETON_SCOPE( Logger );

Logger::Logger()
  : mFile( NULL ), mProcessMessages( false )
{
}

Logger::~Logger() {
  if( mFile ) {
    delete mFile;
    mFile = NULL;
  }
}

void Logger::StartProcessing() {
  mProcessMessages = true;
  ProcessMessages();
}

void Logger::SetLogPath( const QString& path ) {
  if( mFile )
    delete mFile;

  mFile = new QFile( path );
  if( mFile )  {
    mFile->open( QIODevice::WriteOnly | QIODevice::Text );
  }
}

void Logger::ProcessMessages() {
  if( !mProcessMessages )
    return;

  for( auto it : mQueue ) {
    LogEventType type = it.first;
    const QString& msg = it.second;

    if( mFile && mFile->isOpen() ) {
      QTextStream out( mFile );
      out << msg;
      mFile->flush();
    }

    emit NewMessage( type, msg );
  }

  mQueue.clear();
}

void Logger::Add( LogEventType type, const char *fmt, ... ) {
  char buffer[ 4096 ];

  // Parse vargs
  va_list args;
  va_start( args, fmt );
  vsnprintf( buffer, sizeof(buffer), fmt, args );
  va_end( args );

  // Timestamp
  char timestamp[ 256 ];
  time_t t = time( 0 );
  struct tm *now = localtime( &t );
  strftime( timestamp, sizeof( timestamp ), "%H:%M:%S", now );

  // Decorate
  char decorated[ 4096 ];
  sprintf( decorated, "[%s] %s: %s\n", timestamp, LOG_EVENT_TYPE_NAMES[ type ], buffer );
  QString line( decorated );

  mQueue.push_back( QPair< LogEventType, QString >( type, line ) );
  ProcessMessages();
}
