#include "Local.h"

#define MAX_STR_CACHED 10
const char *qt2cstr( const QString& str ) {
  static QByteArray byteArray[ MAX_STR_CACHED ];
  static int cnt = 0;

  if( ++cnt >= MAX_STR_CACHED )
    cnt = 0;

  byteArray[ cnt ] = str.toLatin1();
  return byteArray[ cnt ].constData();
}
