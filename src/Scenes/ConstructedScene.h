#pragma once

#include "Scene.h"

class ConstructedScene : public Scene
{
private:
  ::GameMode mMode;

public:
  ConstructedScene()
    : Scene( "Constructed" )
  {
    ADD_GENERATED_MARKER( "constructed_casual_selected", CONSTRUCTED_CASUAL_ID );
    ADD_GENERATED_MARKER( "constructed_ranked_selected", CONSTRUCTED_RANKED_ID );

    Reset();
  }

  void Reset() {
    Scene::Reset();

    mMode = MODE_UNKNOWN;
  }

  void Update() {
    if( FindMarker( "constructed_casual_selected") ) {
      mMode = MODE_CASUAL;
    }
    if( FindMarker( "constructed_ranked_selected") ) {
      mMode = MODE_RANKED;
    }
  }

  bool Active() {
    return FindMarker( "constructed_casual_selected" ) || FindMarker( "constructed_ranked_selected" );
  }

  ::GameMode GameMode() const {
    return mMode;
  }
};

