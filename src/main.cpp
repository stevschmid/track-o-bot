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
