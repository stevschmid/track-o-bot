#include <QApplication.h>
#include <QDir>

#include "hearthstone.h"
#include "dhasher.h"
#include "scene_manager.h"

#include "window.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  Window window;
  window.show();
  return app.exec();
}

/* int main(int argc, char **argv) */
/* { */
/*   QApplication app(argc, argv); */
/*   QDir::setCurrent(app.applicationDirPath()); */

/*   OSXWindowCapture capture("Hearthstone"); */
/*   Hearthstone::Instance()->SetWindowCapture(&capture); */

/*   SceneManager sceneManager; */

/*   while(1) { */
/*     sceneManager.Update(); */
/*     const Scene *activeScene = sceneManager.ActiveScene(); */
/*     if(activeScene) { */
/*       cout << "ActiveScene " << activeScene->GetName() << endl; */
/*     } */
/*     usleep(100000); */
/*   } */

/*   return 0; */
/* } */
