#pragma once

#include "scene_manager.h"
#include "tracker.h"

#include <QTimer>

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
  QTimer *timer;

  SceneManager sceneManager;
  GameMode currentGameMode;

private slots:
  void Tick();

public:
  Core();
  ~Core();

  void SceneChanged(Scene *oldScene, Scene *newScene);
};
