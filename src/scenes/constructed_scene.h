#pragma once

#include "scene.h"

class ConstructedScene : public Scene
{
private:
  GameMode mode;

public:
  ConstructedScene()
    :Scene("Constructed")
  {
    ADD_GENERATED_MARKER("constructed_casual_selected", CONSTRUCTED_CASUAL_ID);
    ADD_GENERATED_MARKER("constructed_ranked_selected", CONSTRUCTED_RANKED_ID);
    ADD_GENERATED_MARKER("constructed_friendly", CONSTRUCTED_FRIENDLY_ID);
    ADD_GENERATED_MARKER("constructed_practice_selection", CONSTRUCTED_PRACTICE_SELECTION_ID);
  }

  void Init() {
    mode = MODE_UNKNOWN;
  }

  void Update() {
    if(FindMarker("constructed_casual_selected")) {
      mode = MODE_CASUAL;
    }
    if(FindMarker("constructed_ranked_selected")) {
      mode = MODE_RANKED;
    }
    if(FindMarker("constructed_friendly")) {
      mode = MODE_FRIENDLY;
    }

    // practice and friendly have the same layout,
    // except for the opponent selection
    // (which is shown after we press "Play")
    if(FindMarker("constructed_practice_selection")) {
      mode = MODE_PRACTICE;
    }
  }

  bool Active() {
    return FindMarker("constructed_casual_selected") ||
           FindMarker("constructed_ranked_selected") ||
           FindMarker("constructed_friendly") ||
           FindMarker("constructed_practice_selection");
  }

  GameMode GetGameMode() const {
    return mode;
  }
};

