#pragma once

#include "scene.h"

class MainMenuScene : public Scene
{
public:
  MainMenuScene():Scene("MainMenu") {
    ADD_GENERATED_MARKER("main_menu", MAIN_MENU_ID);
  }

  bool Active() {
    return FindMarker("main_menu");
  }
};
