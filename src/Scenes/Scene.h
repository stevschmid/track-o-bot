#pragma once

#include "../Phash.h"
#include "../Hearthstone.h"
#include "../GeneratedMarkers.h"

#include <QPixmap>

#define ADD_GENERATED_MARKER(NAME, DEF) AddMarker(NAME, DEF##_PATH, DEF##_X, DEF##_Y, DEF##_WIDTH, DEF##_HEIGHT)

class Scene
{
private:
  struct Marker {
    string name;
    int x, y, w, h; // virtual canvas attributes!
    Phash hash;

    Marker() : name( "" ), x( 0 ), y( 0 ), w( 0 ), h( 0 ) {
    }

    Marker( const string& name, const string& templateImagePath, int x, int y, int w, int h )
      : name( name ), x( x ), y( y ), w( w ), h( h ), hash( QPixmap( templateImagePath.c_str() ) )
    {
    }
  };

  map< string, Marker > mMarkers;
#ifdef _DEBUG
  map< string, bool > mDebugMarkerFound;
#endif

  string mName;
  int mOriginX, mOriginY;

public:
  Scene( const string& name )
    : mName( name ), mOriginX( 0 ), mOriginY( 0 )
  {
    Reset();
  }

  virtual ~Scene() {
  }

  void AddMarker( const string& name, const string& templateImagePath, int x, int y, int w, int h ) {
    mMarkers[ name ] = Marker( name, templateImagePath, x, y, w, h );
  }

  bool FindMarker( const string& name, int dx = 0, int dy = 0 ) {
    const Marker& marker = mMarkers[ name ];
    const QPixmap& capture = Hearthstone::Instance()->Capture(
        mOriginX + marker.x + dx,
        mOriginY + marker.y + dy,
        marker.w,
        marker.h );
    Phash currentHash( capture );
    bool found = currentHash == marker.hash;
#ifdef _DEBUG
    if( mDebugMarkerFound[ name ] != found ) {
      LOG( "Marker %s in Scene %s %s", name.c_str(), mName.c_str(), found ? "found" : "disappeared" );
      mDebugMarkerFound[ name ] = found;
    }
#endif
    return found;
  }

  virtual bool Active() = 0;

  virtual void Reset() {
    SetOrigin( 0, 0 );
  }

  virtual void Update() {
  }

  void SetOrigin( int x, int y ) {
    mOriginX = x;
    mOriginY = y;
  }

  int OriginX() {
    return mOriginX;
  }

  int OriginY() {
    return mOriginY;
  }

  const string& Name() const {
    return mName;
  }
};
