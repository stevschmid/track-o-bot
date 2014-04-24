#pragma once

#include "scene.h"

typedef enum {
  CONSTRUCTED_MODE_UNKNOWN,
  CONSTRUCTED_MODE_CASUAL,
  CONSTRUCTED_MODE_RANKED,
  CONSTRUCTED_MODE_PRACTICE
} ConstructedMode;

class ConstructedScene : public Scene
{
public:
  ConstructedMode mode;

public:
  ConstructedScene()
    :Scene("Constructed")
  {
    AddMarker("constructed_casual_selected", "../scenes/constructed_casual.png", 687, 91, 140, 126);
    AddMarker("constructed_ranked_selected", "../scenes/constructed_ranked.png", 928, 66, 20, 115);
    AddMarker("constructed_practice_selected", "../scenes/constructed_practice.png", 726, 125, 52, 52);
  }

  void Init() {
    mode = CONSTRUCTED_MODE_UNKNOWN;
  }

  void Update() {
    if(FindMarker("constructed_casual_selected")) {
      mode = CONSTRUCTED_MODE_CASUAL;
    }
    if(FindMarker("constructed_ranked_selected")) {
      mode = CONSTRUCTED_MODE_RANKED;
    }
    if(FindMarker("constructed_practice_selected")) {
      mode = CONSTRUCTED_MODE_PRACTICE;
    }
  }

  bool Active() {
    return FindMarker("constructed_casual_selected") ||
           FindMarker("constructed_ranked_selected") ||
           FindMarker("constructed_practice_selected");
  }
};

