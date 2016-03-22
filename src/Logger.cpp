#include "Logger.h"
#include "Settings.h"

#include <QTime>
#include <QTextStream>
#include <cassert>

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
  // Ignore debug events when debug is not enabled
  if( type == LOG_DEBUG && !Settings::Instance()->DebugEnabled() )
    return;

  char buffer[ 4096 ];
  memset( buffer, 0, sizeof( buffer ) );

  va_list args;
  va_start( args, fmt );
  vsnprintf( buffer, sizeof( buffer ) - 1, fmt, args );
  va_end( args );

  QString timestamp = QTime::currentTime().toString( "hh:mm:ss" );
  QString line = QString( "[%1] %2: %3\n" ).arg( timestamp ).arg( LOG_EVENT_TYPE_NAMES[ type ] ).arg( buffer );

  mQueue.push_back( QPair< LogEventType, QString >( type, line ) );
  ProcessMessages();
}
