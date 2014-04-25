#include "core.h"

Core::Core()
  :capture("Hearthstone")
{
  Hearthstone::Instance()->SetWindowCapture(&capture);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(Tick()));
  timer->start(100);
}

Core::~Core() {
  delete timer;
}

void Core::Tick() {
  sceneManager.Update();
  const Scene *activeScene = sceneManager.ActiveScene();
  if(activeScene) {
    cout << "ActiveScene " << activeScene->GetName() << endl;
  }
}
