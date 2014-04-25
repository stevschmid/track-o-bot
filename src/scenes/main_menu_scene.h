#pragma once

#include "scene.h"

class MainMenuScene : public Scene
{
public:
  MainMenuScene():Scene("MainMenu") {
    AddMarker("main_menu", ":/scenes/main_menu.png", 86, 0, 218, 218);
  }

  bool Active() {
    return FindMarker("main_menu");
  }

  void Init() { }
  void Update() { }
};
