#include <QApplication.h>
#include <QDir>

#include <QDesktopServices>

#include "file_logger.h"
#include "window.h"
#include "tracker.h"

Logger logger;

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  app.setApplicationName("Arenatracker"); // for proper DataLocation handling

  QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
  if(!QFile::exists(directory)) {
    QDir dir;
    dir.mkpath(directory);
  }
  std::string logFilePath = (directory + QDir::separator() + "log").toStdString();

  FileLogger fileLogger(logFilePath);
  logger.RegisterObserver(&fileLogger);

  Window window;
  return app.exec();

  logger.UnregisterObserver(&fileLogger);
}
