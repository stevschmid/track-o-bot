#pragma once

#include "scene_manager.h"
#include "osx_window_capture.h"
#include "tracker.h"

#include <QTimer>

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
  QTimer *timer;

  SceneManager sceneManager;
  GameMode currentGameMode;
  Tracker tracker;

private slots:
  void Tick();

public:
  Core();
  ~Core();

  void OpenProfile() { tracker.OpenProfile(); }
  void SceneChanged(Scene *oldScene, Scene *newScene);
};
