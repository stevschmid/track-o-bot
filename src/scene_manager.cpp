#include "scene_manager.h"
#include "tracker.h"

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
    if(currentScene) {
      Transition(currentScene, newScene);
    }
    currentScene = newScene;
  }
}

void SceneManager::Transition(Scene *oldScene, Scene *newScene) {
  cout << "Transition from " << oldScene->GetName() << " to " << newScene->GetName() << endl;

  if(newScene->GetName() ==  "Ingame") {
    IngameScene *ingame = (IngameScene*)newScene;

    if(oldScene->GetName() == "Constructed") {
      ConstructedScene *constructed = (ConstructedScene*)oldScene;
      ingame->SetGameMode(constructed->GetGameMode());
    }
  }

  if(oldScene->GetName() == "Ingame") {
    IngameScene *ingame = (IngameScene*)oldScene;
    Tracker::Instance()->AddResult(ingame->GetGameMode(),
                                   ingame->GetOutcome(),
                                   ingame->GetCoin(),
                                   ingame->GetOwnClass(),
                                   ingame->GetOpponentClass());
  }
}

const Scene* SceneManager::ActiveScene() {
  return currentScene;
}
