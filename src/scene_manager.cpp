#include "scene_manager.h"
#include "tracker.h"

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
  if(currentScene) {
    currentScene->Update();
  }

  Scene *newScene = FindActiveScene();
  if(newScene && newScene != currentScene) {
    newScene->Init();
    Notify(currentScene, newScene);
    currentScene = newScene;
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
