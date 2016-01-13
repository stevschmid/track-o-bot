#include "Metadata.h"

DEFINE_SINGLETON_SCOPE( Metadata );

Metadata::Metadata() {
}

Metadata::~Metadata() {
}

void Metadata::Add( const QString& key, const QString& value ) {
  mMetadata[ key ] = value;
}

void Metadata::Add( const QString& key, const char* fmt, ... ) {
  char buffer[ 4096 ];

  // Parse vargs
  va_list args;
  va_start( args, fmt );
  vsnprintf( buffer, sizeof(buffer), fmt, args );
  va_end( args );

  Add( key, QString( buffer ) );
}

void Metadata::Remove( const QString& key ) {
  mMetadata.remove( key );
}

void Metadata::Clear() {
  mMetadata.clear();
}
