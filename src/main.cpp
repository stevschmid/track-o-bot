#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#include <QDate>
#include <QIcon>
#include <QPointer>

#include <QSharedMemory>

#include "window.h"

#include "updater.h"
#ifdef Q_WS_MAC
#include "cocoa_initializer.h"
#include "sparkle_updater.h"
#endif

int main(int argc, char **argv)
{
  // Enforce single instance
  QSharedMemory memSingleInstance("TrackOBot");
  if(!memSingleInstance.create(512, QSharedMemory::ReadWrite)) {
    return 1;
  }

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

  Updater *updater = NULL;
#ifdef Q_WS_MAC
  CocoaInitializer cocoaInitiarizer;
  updater = new SparkleUpdater("http://andymatuschak.org/files/sparkletestcast.xml");
#endif

  // Initalize Windows n stuff
  Window window;

  // Main Loop
  int exitCode = app.exec();

  // Tear down
  LOG("<-- Shutdown");

  return exitCode;
}
