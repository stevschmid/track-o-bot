#pragma once

#include "SceneManager.h"
#include "Tracker.h"
#include "HearthstoneLogTracker.h"

#include <QTimer>
#include <QElapsedTimer>

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
  QTimer*               mTimer;
  QElapsedTimer*        mDebugTimer;
  SceneManager          mSceneManager;
  GameMode              mCurrentGameMode;
  bool                  mGameRunning;
  HearthstoneLogTracker mLogTracker;

private slots:
  void Tick();

public:
  Core();
  ~Core();

  void SceneChanged( Scene *oldScene, Scene *newScene );
};
