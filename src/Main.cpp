#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#include <QDate>
#include <QIcon>
#include <QPointer>

#include <QLocalServer>
#include <QLocalSocket>

#include "Window.h"
#include "Tracker.h"

#include "Updater.h"

#if defined Q_WS_MAC
#include "CocoaInitializer.h"
#include "SparkleUpdater.h"
#elif defined Q_WS_WIN
#include "WinSparkleUpdater.h"
#endif

#include "Hearthstone.h"

Updater *gUpdater = NULL;

int main( int argc, char **argv )
{
#ifdef Q_OS_MACX
  if( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 ) {
    QFont::insertSubstitution(".Helvetica Neue DeskInterface", "Helvetica Neue"); // Fix yosemite text in buttons etc.
  }
#endif

  // Basic setup
  QApplication app( argc, argv );
#if defined Q_WS_MAC
  app.setAttribute( Qt::AA_UseHighDpiPixmaps );
  QIcon icon = QIcon( ":/icons/mac_black.png" );
  icon.addFile( ":/icons/mac_black@2x.png" );
#elif defined Q_WS_WIN
  QIcon icon = QIcon( ":/icons/win.ico" );
#endif
  app.setApplicationName( "Track-o-Bot" ); // for proper DataLocation handling
  app.setOrganizationName( "spidy.ch" );
  app.setOrganizationDomain( "spidy.ch" );
  app.setWindowIcon( icon );

  // Enforce single instance
  const char serverName[] = "trackobot";

  QLocalSocket socket;
  socket.connectToServer( serverName );
  if( socket.waitForConnected(500) ) {
    return 1; // already running
  }

  QLocalServer::removeServer( serverName );
  QLocalServer server( NULL );
  if( !server.listen(serverName) ) {
    return 2;
  }

  // Logging
  QString dataLocation = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
  if( !QFile::exists(dataLocation) ) {
    QDir dir;
    dir.mkpath( dataLocation );
  }
  string logFilePath = ( dataLocation + QDir::separator() + app.applicationName() + ".log" ).toStdString();
  Logger::Instance()->SetLogPath( logFilePath );

  // Start
  LOG( "--> Launched v%s on %s", VERSION, QDate::currentDate().toString( Qt::ISODate ).toStdString().c_str() );

#if defined Q_WS_MAC
  CocoaInitializer cocoaInitializer;
  gUpdater = new SparkleUpdater( Tracker::Instance()->WebserviceURL( "/appcast.xml" ) );
#elif defined Q_WS_WIN
  gUpdater = new WinSparkleUpdater( Tracker::Instance()->WebserviceURL( "/appcast_win.xml" ) );
#endif

  // Initalize Windows n stuff
  Window window;

  // Make sure Account exists or create one
  Tracker::Instance()->EnsureAccountIsSetUp();

  // Enable HS Logging
  Hearthstone::Instance()->EnableLogging();

  // Main Loop
  int exitCode = app.exec();

  // Tear down
  LOG( "<-- Shutdown" );

  return exitCode;
}
