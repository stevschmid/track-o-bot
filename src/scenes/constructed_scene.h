#pragma once

#include "scene.h"

class ConstructedScene : public Scene
{
protected:
  GameMode mode;

public:
  ConstructedScene()
    :Scene("Constructed")
  {
    AddMarker("constructed_casual_selected", ":/scenes/constructed_casual.png", 796, 24, 59, 65);
    AddMarker("constructed_ranked_selected", ":/scenes/constructed_ranked.png", 796, 24, 59, 65);
    AddMarker("constructed_practice_selected", ":/scenes/constructed_practice.png", 726, 125, 52, 52);
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
    if(FindMarker("constructed_practice_selected")) {
      mode = MODE_PRACTICE;
    }
  }

  bool Active() {
    return FindMarker("constructed_casual_selected") ||
           FindMarker("constructed_ranked_selected") ||
           FindMarker("constructed_practice_selected");
  }

  GameMode GetGameMode() const {
    return mode;
  }
};

