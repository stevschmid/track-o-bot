#include "SceneManager.h"
#include "Tracker.h"

SceneManager::SceneManager()
  :currentScene(NULL)
{
  RegisterScene(new MainMenuScene);
  RegisterScene(new ConstructedScene);
  RegisterScene(new IngameScene);
  RegisterScene(new ArenaScene);
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

Scene* SceneManager::FindActiveScene() {
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
  bool findNewScene = (currentScene == NULL);

  if(currentScene) {
    currentScene->Update();
    findNewScene |= !currentScene->Active();
  }

  // Keep the current scene if no other scene is detected yet.
  // Scene should never be NULL except at the beginning.
  if(findNewScene) {
    Scene *newScene = FindActiveScene();
    if(newScene && newScene != currentScene) {
      // Notify our dear observers
      Notify(currentScene, newScene);

      // Make sure we reset the previous' scene state (e.g. origin)
      if(currentScene) {
        currentScene->Reset();
      }

      // Switch to the new scene
      currentScene = newScene;
    }
  }
}

const Scene* SceneManager::GetActiveScene() {
  return currentScene;
}

void SceneManager::RegisterObserver(SceneManagerObserver *observer) {
  observers.push_back(observer);
}

void SceneManager::UnregisterObserver(SceneManagerObserver *observer) {
  observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
}

void SceneManager::Notify(Scene *oldScene, Scene *newScene) {
  vector<SceneManagerObserver*>::iterator it;
  for(it = observers.begin(); it != observers.end(); ++it) {
    (*it)->SceneChanged(oldScene, newScene);
  }
}
