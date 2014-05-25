#include "core.h"

#include "tracker.h"

Core::Core()
  :currentGameMode(MODE_UNKNOWN), gameRunning(false)
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

  bool wasGameRunning = gameRunning;
  gameRunning = Hearthstone::Instance()->IsRunning();

  if(wasGameRunning != gameRunning) {
    if(gameRunning) {
      LOG("Hearthstone started");
    } else {
      LOG("Hearthstone was closed");
    }
  }

  if(gameRunning) {
    sceneManager.Update();
  }
}

void Core::SceneChanged(Scene *oldScene, Scene *newScene) {
  LOG("Scene %s", newScene->GetName().c_str());

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

    Tracker::Instance()->AddResult(currentGameMode,
        ingame->GetOutcome(),
        ingame->GetGoingOrder(),
        ingame->GetOwnClass(),
        ingame->GetOpponentClass(),
        ingame->GetCardHistoryList());
  }
}
