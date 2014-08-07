#pragma once

#include "Scene.h"

class ConstructedScene : public Scene
{
private:
  GameMode mMode;

public:
  ConstructedScene()
    : Scene( "Constructed" )
  {
    ADD_GENERATED_MARKER( "constructed_casual_selected", CONSTRUCTED_CASUAL_ID );
    ADD_GENERATED_MARKER( "constructed_ranked_selected", CONSTRUCTED_RANKED_ID );
    ADD_GENERATED_MARKER( "constructed_friendly", CONSTRUCTED_FRIENDLY_ID );
    ADD_GENERATED_MARKER( "constructed_practice_selection", CONSTRUCTED_PRACTICE_SELECTION_ID );

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
    if( FindMarker( "constructed_friendly") ) {
      mMode = MODE_FRIENDLY;
    }

    // practice and friendly have the same layout,
    // except for the opponent selection
    // (which is shown after we press "Play")
    if( FindMarker( "constructed_practice_selection" ) ) {
      mMode = MODE_PRACTICE;
    }
  }

  bool Active() {
    return FindMarker( "constructed_casual_selected" ) ||
           FindMarker( "constructed_ranked_selected" ) ||
           FindMarker( "constructed_friendly" ) ||
           FindMarker( "constructed_practice_selection" );
  }

  GameMode GameMode() const {
    return mMode;
  }
};

