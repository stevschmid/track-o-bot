#pragma once

#include "SceneManager.h"
#include "Tracker.h"

#include <QTimer>

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
  QTimer *timer;

  SceneManager sceneManager;
  GameMode currentGameMode;

  bool gameRunning;

private slots:
  void Tick();

public:
  Core();
  ~Core();

  void SceneChanged(Scene *oldScene, Scene *newScene);
};
