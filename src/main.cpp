#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#include <QSettings>
#include <QDate>
#include <QIcon>
#include <QPointer>

#include "window.h"
#include "tracker.h"

QPointer<Window> window;

void redirectLogMessages(QtMsgType type, const char *msg)
{
  if(window) {
    window->addLogEntry(type, msg);
  }
}

int main(int argc, char **argv)
{
  qInstallMsgHandler(redirectLogMessages);

  // Basic setup
  QApplication app(argc, argv);
  QIcon icon = QIcon(":/icons/paw.svg");
  app.setApplicationName(APP_NAME); // for proper DataLocation handling
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
  LOG("--> Launched On %s", QDate::currentDate().toString(Qt::ISODate).toStdString().c_str());

  // Initalize Windows n stuff
  window = new Window();

  // Main Loop
  int exitCode = app.exec();

  // Tear down
  LOG("<-- Shutdown");

  delete window;

  return exitCode;
}
