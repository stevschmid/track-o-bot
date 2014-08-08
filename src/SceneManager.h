#pragma once

#include "Scenes/Scene.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/ConstructedScene.h"
#include "Scenes/ArenaScene.h"
#include "Scenes/IngameScene.h"
#include "Scenes/SoloAdventuresScene.h"
#include "Scenes/ClassSelectionScene.h"

class SceneManagerObserver {
public:
  virtual void SceneChanged( Scene *oldScene, Scene *newScene ) = 0;
};

class SceneManager
{
private:
  vector< SceneManagerObserver* > mObservers;
  vector< Scene* >                mScenes;
  Scene*                          mCurrentScene;

  void    RegisterScene( Scene *scene );
  Scene*  FindActiveScene();
  void    Notify( Scene *oldScene, Scene *newScene );

public:
  SceneManager();
  ~SceneManager();

  void RegisterObserver( SceneManagerObserver *observer );
  void UnregisterObserver( SceneManagerObserver *observer );

  void Update();

  const Scene* ActiveScene() const;
};
