#pragma once

#include "SceneManager.h"
#include "Tracker.h"
#include "HearthstoneLogTracker.h"

#define CORE_POLLING_RATE_IN_MS    50

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
  SceneManager          mSceneManager;
  GameMode              mCurrentGameMode;
  bool                  mGameRunning;
  HearthstoneLogTracker mLogTracker;

private slots:
  void Tick();
  void Loop();

public:
  Core();
  ~Core();

  void SceneChanged( Scene *oldScene, Scene *newScene );
};
