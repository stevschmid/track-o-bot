#include "Logger.h"
#include <ctime>

DEFINE_SINGLETON_SCOPE( Logger );

Logger::Logger() {
}

Logger::~Logger() {
}


void Logger::SetLogPath( const string& path ) {
  mOf.open( path.c_str(), std::ios_base::app );
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
  string line = string( decorated );

  // Add to file
  if( mOf.is_open() ) {
    mOf << line;
    mOf.flush();
  }

  emit NewMessage( type, line );
}
