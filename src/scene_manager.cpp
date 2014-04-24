#include "scene_manager.h"

SceneManager::SceneManager()
  :currentScene(NULL)
{
  RegisterScene(new MainMenuScene);
  RegisterScene(new ConstructedScene);
  RegisterScene(new IngameScene);
}

SceneManager::~SceneManager() {
  vector<Scene*>::iterator it;
  for(it = scenes.begin(); it != scenes.end(); ++it) {
    delete *it;
  }
}

void SceneManager::RegisterScene(Scene *scene) {
  scenes.push_back(scene);
}

Scene* SceneManager::findActiveScene() {
  vector<Scene*>::iterator it;
  for(it = scenes.begin(); it != scenes.end(); it++) {
    Scene *scene = *it;
    if(scene->Active()) {
      return scene;
    }
  }
  return NULL;
}

void SceneManager::Update() {
  if(currentScene) {
    currentScene->Update();
  }

  Scene *newScene = findActiveScene();
  if(newScene && newScene != currentScene) {
    newScene->Init();
    if(currentScene) {
      Transition(currentScene, newScene);
    }
    currentScene = newScene;
  }
}

void SceneManager::Transition(Scene *oldScene, Scene *newScene) {
  cout << "Transition from " << oldScene->GetName() << " to " << newScene->GetName() << endl;
  if(newScene->GetName() ==  "Ingame") {
    if(oldScene->GetName() == "Constructed") {
      /* newScene->setGameMode(); */
    }
  }

  if(newScene->GetName() == "Constructed") {
    if(oldScene->GetName() == "Ingame") {
      /* Tracker::Instance()->AddResult(oldScene->getGameMode(), */
      /*                                oldScene->getOutcome(), */
      /*                                oldScene->getOwnClass(), */
      /*                                oldScene->getOpponentClass()); */
    }
  }
}

const Scene* SceneManager::ActiveScene() {
  return currentScene;
}
