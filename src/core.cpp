#include "core.h"

#include "tracker.h"

Core::Core()
  :currentGameMode(MODE_UNKNOWN)
{
  sceneManager.RegisterObserver(this);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(Tick()));
  timer->start(100);
}

Core::~Core() {
  delete timer;
}

void Core::Tick() {
  if(Hearthstone::Instance()->IsRunning()) {
    sceneManager.Update();
  }
}

void Core::SceneChanged(Scene *oldScene, Scene *newScene) {
#ifdef _DEBUG
  LOG("Detected Scene %s", newScene->GetName().c_str());
#endif

  if(newScene->GetName() ==  "Ingame") {
    if(oldScene) {
      if(oldScene->GetName() == "Constructed") {
        ConstructedScene *constructed = (ConstructedScene*)oldScene;
        currentGameMode = constructed->GetGameMode();
      }
      if(oldScene->GetName() == "Arena") {
        currentGameMode = MODE_ARENA;
      }
    }
  }

  if(oldScene && oldScene->GetName() == "Ingame") {
    IngameScene *ingame = (IngameScene*)oldScene;
    tracker.AddResult(currentGameMode,
        ingame->GetOutcome(),
        ingame->GetGoingOrder(),
        ingame->GetOwnClass(),
        ingame->GetOpponentClass());
  }
}
