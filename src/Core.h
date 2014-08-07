#pragma once

#include "SceneManager.h"
#include "Tracker.h"

#include <QTimer>

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
  QTimer*       mTimer;
  SceneManager  mSceneManager;
  GameMode      mCurrentGameMode;
  bool          mGameRunning;

private slots:
  void Tick();

public:
  Core();
  ~Core();

  void SceneChanged( Scene *oldScene, Scene *newScene );
};
