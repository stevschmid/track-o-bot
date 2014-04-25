#pragma once

#include "scene_manager.h"
#include "osx_window_capture.h"

#include <QTimer>

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
    QTimer *timer;

    SceneManager sceneManager;
    OSXWindowCapture capture;

    GameMode gameMode;

private slots:
    void Tick();

public:
    Core();
    ~Core();

    void SceneChanged(Scene *oldScene, Scene *newScene);
};
