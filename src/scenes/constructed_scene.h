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
    ADD_GENERATED_MARKER("constructed_practice_or_friendly", CONSTRUCTED_PRACTICE_OR_FRIENDLY_ID);
    ADD_GENERATED_MARKER("constructed_practice_select_opponent", CONSTRUCTED_PRACTICE_SELECT_OPPONENT_ID);
  }

  void Init() {
    mode = MODE_UNKNOWN;
  }

  void Update() {
    if(mode == MODE_UNKNOWN) {
      if(FindMarker("constructed_casual_selected")) {
        mode = MODE_CASUAL;
      }
      if(FindMarker("constructed_ranked_selected")) {
        mode = MODE_RANKED;
      }
      if(FindMarker("constructed_practice_or_friendly")) {
        mode = MODE_FRIENDLY;
      }
    }

    // practice and friendly have the same layout,
    // except for the opponent selection
    // (which is shown after we press "Play")
    if(mode == MODE_FRIENDLY && FindMarker("constructed_practice_select_opponent")) {
      mode = MODE_PRACTICE;
    }
  }

  bool Active() {
    return FindMarker("constructed_casual_selected") ||
           FindMarker("constructed_ranked_selected") ||
           FindMarker("constructed_practice_or_friendly");
  }

  GameMode GetGameMode() const {
    return mode;
  }
};

