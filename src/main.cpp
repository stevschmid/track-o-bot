#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>

#include "file_logger.h"
#include "window.h"
#include "tracker.h"

// Global logger
Logger gLogger;

int main(int argc, char **argv)
{
  // Basic setup
  QApplication app(argc, argv);
  QIcon icon = QIcon(":/icons/paw.svg");
  app.setApplicationName(APP_NAME); // for proper DataLocation handling
  app.setOrganizationName("spidy.ch");
  app.setOrganizationDomain("spidy.ch");
  app.setWindowIcon(icon);

  // Logging
  // Non-generic DataLocation includes the organization name, which we don't want
  QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
  if(!QFile::exists(dataLocation)) {
    QDir dir;
    dir.mkpath(dataLocation);
  }
  string logFilePath = (dataLocation + QDir::separator() + app.applicationName() + QDir::separator() + app.applicationName() + ".log").toStdString();
  FileLogger fileLogger(logFilePath);
  gLogger.RegisterObserver(&fileLogger);

  // Initalize Windows n stuff
  Window window;

  // Main Loop
  int exitCode = app.exec();

  // Tear down
  gLogger.UnregisterObserver(&fileLogger);

  return exitCode;
}
