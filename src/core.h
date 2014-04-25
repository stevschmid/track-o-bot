#pragma once

#include "scene_manager.h"
#include "osx_window_capture.h"

#include <QTimer>

class Core : public QObject
{
  Q_OBJECT

private:
    QTimer *timer;

    SceneManager sceneManager;
    OSXWindowCapture capture;

private slots:
    void Tick();

public:
    Core();
    ~Core();
};
