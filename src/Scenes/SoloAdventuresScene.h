#pragma once

#include "Scene.h"

class SoloAdventuresScene : public Scene
{
public:
  SoloAdventuresScene()
    : Scene( "SoloAdventures" )
  {
    ADD_GENERATED_MARKER( "solo_adventures", SOLO_ADVENTURES_ID );
  }

  bool Active() {
    return FindMarker( "solo_adventures" );
  }
};
