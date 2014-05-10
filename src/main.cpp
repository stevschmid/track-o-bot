#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#include <QDate>
#include <QIcon>
#include <QPointer>

#include "window.h"

int main(int argc, char **argv)
{
  // Basic setup
  QApplication app(argc, argv);
  QIcon icon = QIcon(":/icons/tray_icon.png");
  app.setApplicationName("Track-o-Bot"); // for proper DataLocation handling
  app.setOrganizationName("spidy.ch");
  app.setOrganizationDomain("spidy.ch");
  app.setWindowIcon(icon);

  // Logging
  QString dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
  if(!QFile::exists(dataLocation)) {
    QDir dir;
    dir.mkpath(dataLocation);
  }
  string logFilePath = (dataLocation + QDir::separator() + app.applicationName() + ".log").toStdString();
  Logger::Instance()->SetLogPath(logFilePath);

  // Start
  LOG("--> Launched v%s On %s", VERSION, QDate::currentDate().toString(Qt::ISODate).toStdString().c_str());

  // Initalize Windows n stuff
  Window window;

  // Main Loop
  int exitCode = app.exec();

  // Tear down
  LOG("<-- Shutdown");

  return exitCode;
}
