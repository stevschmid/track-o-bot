#include "Hearthstone.h"

#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QSettings>
#include <QTextStream>
#include <QRegularExpression>

#include <QJsonDocument>
#include <QJsonObject>

#ifdef Q_OS_MAC
#include "OSXWindowCapture.h"
#elif defined Q_OS_WIN
#include "WinWindowCapture.h"
#include "Shlobj.h"
#endif

DEFINE_SINGLETON_SCOPE( Hearthstone )

Hearthstone::Hearthstone()
 : mCapture( NULL ), mGameRunning( false )
{
#ifdef Q_OS_MAC
  mCapture = new OSXWindowCapture();
#elif defined Q_OS_WIN
  mCapture = new WinWindowCapture();
#endif

  // On OS X, WindowFound is quite CPU intensive
  // Starting time for HS is also long
  // So just check only once in a while
  mTimer = new QTimer( this );
  connect( mTimer, &QTimer::timeout, this, &Hearthstone::Update );
#ifdef Q_OS_MAC
  mTimer->start( 5000 );
#else
  mTimer->start( 250 );
#endif
}

Hearthstone::~Hearthstone() {
  if( mCapture != NULL )
    delete mCapture;
}

void Hearthstone::Update() {
  bool isRunning = mCapture->WindowFound();

  if( isRunning ) {
    static int lastLeft = 0, lastTop = 0, lastWidth = 0, lastHeight = 0;
    if( lastLeft != mCapture->Left() || lastTop != mCapture->Top() ||
        lastWidth != mCapture->Width() || lastHeight != mCapture->Height() )
    {
      lastLeft = mCapture->Left(),
               lastTop = mCapture->Top(),
               lastWidth = mCapture->Width(),
               lastHeight = mCapture->Height();

      DBG( "HS window changed %d %d %d %d", lastLeft, lastTop, lastWidth, lastHeight );
      emit GameWindowChanged( lastLeft, lastTop, lastWidth, lastHeight );
    }
  }

  if( mGameRunning != isRunning ) {
    mGameRunning = isRunning;

    if( isRunning ) {
      LOG( "Hearthstone is running" );
      emit GameStarted();
      // it takes time to create logfile
      QTimer::singleShot( 3000, this, &Hearthstone::DetectRegion );

    } else {
      LOG( "Hearthstone stopped" );
      emit GameStopped();
      mRegion.clear();
    }
  }
}

QString Hearthstone::ReadAgentAttribute( const char *attributeName ) const {
#ifdef Q_OS_MAC
  QString path = "/Users/Shared/Battle.net/Agent/agent.db";
#elif defined Q_OS_WIN
  wchar_t buffer[ MAX_PATH ];
  SHGetSpecialFolderPathW( NULL, buffer, CSIDL_COMMON_APPDATA, FALSE );
  QString programData = QString::fromWCharArray( buffer );
  QString path = programData + "\\Battle.net\\Agent\\agent.db";
#endif

  QFile file( path );
  if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    DBG( "Couldn't open %s (%d)", qt2cstr( path ), file.error() );
    return "";
  }

  QString contents = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson( contents.toUtf8() );
  QJsonObject root = doc.object();

  QJsonObject hs = root["/game/hs_beta"].toObject()["resource"].toObject()["game"].toObject();
  return hs[ QString( attributeName ) ].toString();
}

bool Hearthstone::GameRunning() const {
  return mGameRunning;
}

#ifdef Q_OS_WIN
inline float roundf( float x ) {
   return x >= 0.0f ? floorf( x + 0.5f ) : ceilf( x - 0.5f );
}
#endif

bool Hearthstone::CaptureWholeScreen( QPixmap *screen ) {
  *screen = mCapture->Capture( 0, 0, Width(), Height() );
  return true;
}

QPixmap Hearthstone::Capture( int canvasWidth, int canvasHeight, int cx, int cy, int cw, int ch )
{
  UNUSED_ARG( canvasWidth );

  int x, y, w, h;

  int windowHeight = mCapture->Height();
  float scale = windowHeight / float( canvasHeight );

  x = roundf( cx * scale );
  y = roundf( cy * scale );

  w = roundf( cw * scale );
  h = roundf( ch * scale );

  return mCapture->Capture( x, y, w, h );
}

void Hearthstone::SetWindowCapture( WindowCapture *windowCapture ) {
  if( mCapture != NULL )
    delete mCapture;

  mCapture = windowCapture;
}

void Hearthstone::EnableLogging() {
  QString path = LogConfigPath();
  QFile file( path );

  bool logModified = false;

  // Read file contents
  QString contents;
  if( file.exists() ) {
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    QTextStream in( &file );
    contents = in.readAll();
    file.close();
  }


  // Check what modules we have to activate
  QStringList modulesToActivate;
  for( int i = 0; i < NUM_LOG_MODULES; i++ ) {
    const char *moduleName = LOG_MODULE_NAMES[ i ];
    QString moduleLine = QString( "[%1]" ).arg( moduleName ) ;
    if( !contents.contains( moduleLine ) ) {
      contents += "\n";
      contents += moduleLine + "\n";
      contents += "LogLevel=1\n";
      contents += "FilePrinting=true\n";

      DBG( "Activate module %s", moduleName );
      logModified = true;
    }
  }

  QRegularExpression regexEnabledConsolePrinting( "ConsolePrinting\\s*=\\s*true",
      QRegularExpression::CaseInsensitiveOption );
  QRegularExpression regexDisabledFilePrinting( "FilePrinting\\s*=\\s*false",
      QRegularExpression::CaseInsensitiveOption );
  if( contents.contains( regexEnabledConsolePrinting ) ||
      contents.contains( regexDisabledFilePrinting ) )
  {
    contents.replace( regexEnabledConsolePrinting, "FilePrinting=true" );
    contents.replace( regexDisabledFilePrinting, "FilePrinting=true" );

    DBG( "FilePrinting enabled" );
    logModified = true;
  }

  // Finally write updated log.config
  if( logModified ) {
    DBG( "Log modified. Write new version" );

    if( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
      ERR( "Couldn't create file %s", qt2cstr( path ) );
    } else {
      QTextStream out( &file );
      out << contents;
    }
  }

  // Notify about restart if game is running
  Update();
  if( GameRunning() && logModified ) {
    emit GameRequiresRestart();
  }
}

void Hearthstone::DisableLogging() {
  QFile file( LogConfigPath() );
  if( file.exists() ) {
    file.remove();
    LOG( "Ingame log deactivated." );
  }
}

QString Hearthstone::LogConfigPath() const {
#ifdef Q_OS_MAC
  QString homeLocation = QStandardPaths::standardLocations( QStandardPaths::HomeLocation ).first();
  QString configPath = homeLocation + "/Library/Preferences/Blizzard/Hearthstone/log.config";
#elif defined Q_OS_WIN
  wchar_t buffer[ MAX_PATH ];
  SHGetSpecialFolderPathW( NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE );
  QString localAppData = QString::fromWCharArray( buffer );
  QString configPath = localAppData + "/Blizzard/Hearthstone/log.config";
#endif
  return configPath;
}

QString Hearthstone::DetectHearthstonePath() const {
  static QString hsPath;

  if( hsPath.isEmpty() ) {
#ifdef Q_OS_WIN
    QString hsPathByAgent = ReadAgentAttribute( "install_dir" );

    QSettings hsKey( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hearthstone", QSettings::NativeFormat );
    QString hsPathByRegistry = hsKey.value( "InstallLocation" ).toString();

    if( hsPathByAgent.isEmpty() && hsPathByRegistry.isEmpty() ) {
      LOG( "Game folder not found. Fall back to default game path for now. You should set the path manually in the settings!" );
      hsPath = QString( getenv("PROGRAMFILES") ) + "/Hearthstone";
    } else if( !hsPathByRegistry.isEmpty() ) {
      hsPath = hsPathByRegistry;
    } else {
      hsPath = hsPathByAgent;
    }
#elif defined Q_OS_MAC
    hsPath = ReadAgentAttribute( "install_dir" );
    if( hsPath.isEmpty() ) {
      LOG( "Fall back to default game path. You should set the path manually in the settings!" );
      hsPath = QStandardPaths::standardLocations( QStandardPaths::ApplicationsLocation ).first() + "/Hearthstone";
    }
#endif
  }

  return hsPath;
}

int Hearthstone::Width() const {
  return mCapture->Width();
}

int Hearthstone::Height() const {
  return mCapture->Height();
}

const QString& Hearthstone::Region() const {
  return mRegion;
}

const QString Hearthstone::DetectRegion() const {

  bool fallback = false;
  QStringList dirList;
  QString region;
  QRegularExpressionMatch match;
  QRegularExpression gameLaunchRegExp( "^.*\\[ProductState\\] {Main} InstallState \\(hs_beta\\): GameLaunching=1" );
  QRegularExpression fallbackHelperRegExp( ".*REGION=(\\w{2})");
  QRegularExpression regionRegExp( "^.*Connection Request to '(\\w{2}).actual.battle.net.*$" );

#ifdef Q_OS_MAC
  QString path = QStandardPaths::standardLocations( QStandardPaths::HomeLocation ).first() + "/Library/Preferences/";
#elif defined Q_OS_WIN
  wchar_t buffer[ MAX_PATH ];
  SHGetSpecialFolderPathW( NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE );
  QString path = QString::fromWCharArray( buffer );
#endif
  dirList  << path + "/Blizzard/Hearthstone/Logs/"
           << path + "/Battle.net/Logs/";

  /* Done in foreach since detection and fallback detection are done almost the same way.
   *
   * For hearthstone log file first match contains info about region.
   * New log file is created every hs launch.
   *
   * For battle.net log file (fallback) the last region is the one we need
   * since they are logged chronologically. We search for every line with
   * (hs_beta) GameLauching=1 and the next line contains info about region. */

  foreach ( const QString& dir, dirList ) {
    QDir logDir( dir, "[hb]*.log", QDir::Time );

    if ( !logDir.exists() || logDir.entryList().isEmpty() ) {
      ERR( "Directory %s does not exist or it is empty!", qt2cstr( logDir.path() ) );
      fallback = true;
      continue;
    }

    QFile logFile( logDir.entryInfoList().first().filePath() );
    if ( !logFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
      ERR( "Cannot open open file: %s!", qt2cstr( logFile.fileName() ) );
      fallback = true;
      continue;
    }

    QTextStream input( &logFile );
    if ( fallback )
       regionRegExp.swap( gameLaunchRegExp );

    do {
      if ( ( match = regionRegExp.match ( input.readLine() ) ).hasMatch() ) {

        if( fallback )
          match = fallbackHelperRegExp.match( input.readLine() );

        region = match.capturedTexts().at( 1 );

        if ( !region.isEmpty() && !fallback )
          break;
      }
    } while ( !input.atEnd() );

    if ( !region.isEmpty() ) {
      break;
    }

    fallback = true;
  }

  if ( region.isEmpty() )  {
    // either user is really persistent in deleating log files or there is
    // something wrong with file permitions/instllation so we default to US;
    LOG( "Could not detect region. falling back to US", qt2cstr( region ) );
    return "US";
  }

  region = region.toUpper();
  LOG( "Detected %s region.", qt2cstr( region ) );
  return region;
}
