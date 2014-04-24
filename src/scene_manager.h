#pragma once

#include "scenes/scene.h"
#include "scenes/main_menu_scene.h"
#include "scenes/constructed_scene.h"
#include "scenes/arena_scene.h"
#include "scenes/ingame_scene.h"

class SceneManager
{
protected:
  Scene *currentScene;
  vector<Scene*> scenes;

  void RegisterScene(Scene *scene);

  Scene *findActiveScene();

public:
  SceneManager();
  ~SceneManager();

  void Update();
  void Transition(Scene *oldScene, Scene *newScene);
  const Scene* ActiveScene();
};
