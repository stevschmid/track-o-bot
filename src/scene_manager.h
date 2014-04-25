#pragma once

#include "scenes/scene.h"
#include "scenes/main_menu_scene.h"
#include "scenes/constructed_scene.h"
#include "scenes/arena_scene.h"
#include "scenes/ingame_scene.h"

class SceneManagerObserver {
public:
  virtual void SceneChanged(Scene *oldScene, Scene *newScene) = 0;
};

class SceneManager
{
protected:
  vector<SceneManagerObserver*> observers;

  Scene *currentScene;
  vector<Scene*> scenes;

  void RegisterScene(Scene *scene);
  Scene *FindActiveScene();
  void Notify(Scene *oldScene, Scene *newScene);

public:
  SceneManager();
  ~SceneManager();

  void RegisterObserver(SceneManagerObserver *observer);
  void UnregisterObserver(SceneManagerObserver *observer);

  void Update();

  const Scene* GetActiveScene();
};
