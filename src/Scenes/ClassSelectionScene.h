#pragma once

#include "Scene.h"

class ClassSelectionScene : public Scene
{
public:
  ClassSelectionScene()
    : Scene( "ClassSelection" )
  {
    ADD_GENERATED_MARKER( "class_selection", CLASS_SELECTION_ID );
  }

  bool Active() {
    return FindMarker( "class_selection" );
  }
};
