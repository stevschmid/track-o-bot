#include <QApplication>
#include <QDir>
#include <QStandardPaths>

#include "file_logger.h"
#include "window.h"
#include "tracker.h"

Logger logger;

int main(int argc, char **argv)
{
  QIcon icon = QIcon(":/icons/paw.svg");
  QApplication app(argc, argv);
  app.setApplicationName(APP_NAME); // for proper DataLocation handling
  app.setWindowIcon(icon);

  QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
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
