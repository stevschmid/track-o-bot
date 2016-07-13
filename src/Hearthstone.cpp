#include "Hearthstone.h"

#include <QFile>
#include <QDesktopServices>
#include <QSettings>
#include <QTextStream>
#include <QRegExp>

#include <QJsonDocument>
#include <QJsonObject>

#include <QDirIterator>
#include <QDateTime>

#ifdef Q_OS_MAC
#include "OSXWindowCapture.h"
#elif defined Q_OS_WIN
#include "WinWindowCapture.h"
#include "Shlobj.h"
#endif

DEFINE_SINGLETON_SCOPE( Hearthstone );

Hearthstone::Hearthstone()
 : mCapture( NULL ), mGameRunning( false ), mGameHasFocus( false )
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
    bool hasFocus = mCapture->HasFocus();
    if( mGameHasFocus != hasFocus ) {
      mGameHasFocus = hasFocus;
      emit FocusChanged( hasFocus );
    }

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
    } else {
      LOG( "Hearthstone stopped" );
      emit GameStopped();
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

  QRegExp regexEnabledConsolePrinting( "ConsolePrinting\\s*=\\s*true",
      Qt::CaseInsensitive );
  QRegExp regexDisabledFilePrinting( "FilePrinting\\s*=\\s*false",
      Qt::CaseInsensitive );
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

QString Hearthstone::DetectRegion() const {
  QString region = "";

#ifdef Q_OS_MAC
  QString homeLocation = QStandardPaths::standardLocations( QStandardPaths::HomeLocation ).first();
  QString path = homeLocation + "/Library/Application Support/Battle.net/";
#elif defined Q_OS_WIN
  wchar_t buffer[ MAX_PATH ];
  SHGetSpecialFolderPathW( NULL, buffer, CSIDL_APPDATA, FALSE );
  QString localAppData = QString::fromWCharArray( buffer );
  QString path = localAppData + "/Battle.net/";
#endif

  QDirIterator it( path, QStringList() << "*.config" );
  uint lastModifiedTime = 0;
  QString lastModifiedPath;
  while( it.hasNext() ) {
    it.next();

    uint modifiedTime = it.fileInfo().lastModified().toTime_t();
    if( modifiedTime > lastModifiedTime ) {
      lastModifiedPath = it.fileInfo().absoluteFilePath();
      lastModifiedTime = modifiedTime;
    }
  }

  if( !lastModifiedPath.isEmpty() ) {
    QFile file(lastModifiedPath);
    LOG( "File %s", qt2cstr( lastModifiedPath ) );
    if( file.open( QIODevice::ReadOnly ) ) {
      QByteArray data = file.readAll();
      QJsonDocument doc = QJsonDocument::fromJson( data );
      QJsonObject root = doc.object();

      region = root["User"].toObject()
        ["Client"].toObject()
        ["PlayScreen"].toObject()
        ["GameFamily"].toObject()
        ["WTCG"].toObject()
        ["LastSelectedGameRegion"].toString();
    } else {
      LOG( "Couldn't open config file %s", qt2cstr( lastModifiedPath ) );
    }
  }

  return region;
}

int Hearthstone::Width() const {
  return mCapture->Width();
}

int Hearthstone::Height() const {
  return mCapture->Height();
}

bool Hearthstone::HasFocus() const {
  return mGameHasFocus;
}
