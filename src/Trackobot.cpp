#include "Trackobot.h"

#include <QDir>
#include <QStandardPaths>

#include <QLocalServer>
#include <QLocalSocket>

#include "Updater.h"
#include "Settings.h"

#if defined Q_OS_MAC
#include "CocoaInitializer.h"
#include "SparkleUpdater.h"
#elif defined Q_OS_WIN
#include "WinSparkleUpdater.h"
#endif

#include "Hearthstone.h"
#include "WebProfile.h"

#include <cassert>

Updater *gUpdater = NULL;

Trackobot::Trackobot( int argc, char **argv )
  : mApp( argc, argv ),
    mWindow( NULL ),
    mOverlay( NULL ),
    mSingleInstanceServer( NULL )
{
  SetupApplication();
  mWebProfile = new WebProfile( this );
  mResultTracker = new ResultTracker( this );
  mLogTracker = new HearthstoneLogTracker( this );
}

Trackobot::~Trackobot() {
  if( mWindow ) {
    delete mWindow;
    mWindow = NULL;
  }

  if( mSingleInstanceServer ) {
    delete mSingleInstanceServer;
    mSingleInstanceServer = NULL;
  }
}

int Trackobot::Run() {
  if( IsAlreadyRunning() )
    return 1;

  LOG( "Launch v%s on %s", VERSION, qt2cstr( QDate::currentDate().toString( Qt::ISODate ) ) );

  SetupUpdater();

  CreateUI();

  WireStuff();

  Initialize();

  SetupLogging();

  int exitCode = mApp.exec();

  // Tear down
  LOG( "Shutdown" );

  return exitCode;
}

bool Trackobot::IsAlreadyRunning() {
  // Enforce single instance
  const char serverName[] = "trackobot";

  QLocalSocket socket;
  socket.connectToServer( serverName );
  if( socket.waitForConnected( 500 ) ) {
    return true;
  }

  QLocalServer::removeServer( serverName );
  mSingleInstanceServer = new QLocalServer( NULL );
  if( !mSingleInstanceServer->listen(serverName) ) {
    return true;
  }

  return false;
}

void Trackobot::SetupApplication() {
#if defined Q_OS_MAC
  mApp.setAttribute( Qt::AA_UseHighDpiPixmaps );
  QIcon icon( ":/icons/mac_black.png" );
  icon.addFile( ":/icons/mac_black@2x.png" );
#elif defined Q_OS_WIN
  QIcon icon( ":/icons/win.ico" );
#endif
  mApp.setApplicationName( "Track-o-Bot" ); // for proper DataLocation handling
  mApp.setOrganizationName( "spidy.ch" );
  mApp.setOrganizationDomain( "spidy.ch" );
  mApp.setWindowIcon( icon );
}

void Trackobot::SetupLogging() {
  // Logging
  QString dataLocation = QStandardPaths::writableLocation( QStandardPaths::DataLocation );
  if( !QFile::exists(dataLocation) ) {
    QDir dir;
    dir.mkpath( dataLocation );
  }
  QString logFilePath = dataLocation + QDir::separator() + mApp.applicationName() + ".log";
  Logger::Instance()->SetLogPath( logFilePath );
  Logger::Instance()->StartProcessing();
}

void Trackobot::SetupUpdater() {
#if defined Q_OS_MAC
  CocoaInitializer cocoaInitializer;
  gUpdater = new SparkleUpdater( mWebProfile->WebserviceURL( "/appcast.xml" ) );
#elif defined Q_OS_WIN
  gUpdater = new WinSparkleUpdater( mWebProfile->WebserviceURL( "/appcast_win.xml" ) );
#endif
}

void Trackobot::CreateUI() {
  mOverlay = new Overlay();
  mWindow = new Window();
}

void Trackobot::WireStuff() {
  // ResultTracker
  connect( mLogTracker, &HearthstoneLogTracker::HandleOutcome, mResultTracker, &ResultTracker::HandleOutcome );
  connect( mLogTracker, &HearthstoneLogTracker::HandleOrder, mResultTracker, &ResultTracker::HandleOrder );
  connect( mLogTracker, &HearthstoneLogTracker::HandleOwnClass, mResultTracker, &ResultTracker::HandleOwnClass ) ;
  connect( mLogTracker, &HearthstoneLogTracker::HandleOpponentClass, mResultTracker, &ResultTracker::HandleOpponentClass );
  connect( mLogTracker, &HearthstoneLogTracker::HandleGameMode, mResultTracker, &ResultTracker::HandleGameMode );
  connect( mLogTracker, &HearthstoneLogTracker::HandleLegend, mResultTracker, &ResultTracker::HandleLegend );
  connect( mLogTracker, &HearthstoneLogTracker::HandleTurn, mResultTracker, &ResultTracker::HandleTurn );
  connect( mLogTracker, &HearthstoneLogTracker::HandleCardHistoryListUpdate, mResultTracker, &ResultTracker::HandleCardHistoryListUpdate );

  connect( mLogTracker, &HearthstoneLogTracker::HandleSpectating, mResultTracker, &ResultTracker::HandleSpectating );
  connect( mLogTracker, &HearthstoneLogTracker::HandleMatchStart, mResultTracker, &ResultTracker::HandleMatchStart );
  connect( mLogTracker, &HearthstoneLogTracker::HandleMatchEnd, mResultTracker, &ResultTracker::HandleMatchEnd );

  // Overlay
  connect( mLogTracker, &HearthstoneLogTracker::HandleCardHistoryListUpdate, mOverlay, &Overlay::HandleCardHistoryListUpdate );
  connect( mLogTracker, &HearthstoneLogTracker::HandleCardDrawHistoryListUpdate, mOverlay, &Overlay::HandleCardDrawHistoryListUpdate );

  // Window
  connect( mWindow, &Window::OpenProfile, mWebProfile, &WebProfile::OpenProfile );
}

void Trackobot::Initialize() {
  assert( mWindow );

  // Make sure Account exists or create one
  mWebProfile->EnsureAccountIsSetUp();

  // Enable HS Logging
  Hearthstone::Instance()->EnableLogging();

  // Create Result Tracker
}

