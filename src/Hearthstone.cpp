#include "Hearthstone.h"

#include <QFile>
#include <QDesktopServices>
#include <QSettings>

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
 : mRestartRequired( false )
{
#ifdef Q_OS_MAC
  mCapture = new OSXWindowCapture( "Hearthstone" );
#elif defined Q_OS_WIN
  mCapture = new WinWindowCapture( "Hearthstone" );
#endif
}

Hearthstone::~Hearthstone() {
  if( mCapture != NULL )
    delete mCapture;
}

bool FindJsonObject( QJsonObject obj, QStringList keys, QJsonObject *out ) {
  if( !keys.empty() ) {
    QString key = keys.front();
    keys.pop_front();

    QJsonObject::iterator it = obj.find( key );
    if( it == obj.end() )
      return false;

    return FindJsonObject( (*it).toObject(), keys, out );
  }

  *out = obj;
  return true;
}

string Hearthstone::ReadAgentAttribute( const char *attributeName ) {
  QString path =  "/Users/Shared/Battle.net/Agent/agent.db";
  QFile file( path );
  if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    ERROR( "Couldn't open %s (%d)", path.toStdString().c_str(), file.error() );
    return "";
  }

  QString contents = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson( contents.toUtf8() );
  QJsonObject root = doc.object();

  QStringList keys;
  keys << "/game/hs_beta" << "resource" << "game";

  QJsonObject hs;
  if( !FindJsonObject( root, keys, &hs ) ) {
    ERROR( "Couldn't find HS key" );
    return "";
  }

  return hs[ QString( attributeName ) ].toString().toStdString();
}

bool Hearthstone::Running() {
  return mCapture->WindowFound();
}

#ifdef Q_OS_WIN
inline float roundf( float x ) {
   return x >= 0.0f ? floorf( x + 0.5f ) : ceilf( x - 0.5f );
}
#endif

QPixmap Hearthstone::Capture( int canvasWidth, int canvasHeight, int cx, int cy, int cw, int ch )
{
  int x, y, w, h;

  int windowHeight = mCapture->Height();
  float scale = windowHeight / float( canvasHeight );

  x = roundf( cx * scale );
  y = roundf( cy * scale );

  w = roundf( cw * scale );
  h = roundf( ch * scale );

  DEBUG("x %d y %d w %d h %d | ch %d wh %d", x, y, w, h, canvasHeight, windowHeight);

  return mCapture->Capture( x, y, w, h );
}

void Hearthstone::SetWindowCapture( WindowCapture *windowCapture ) {
  if( mCapture != NULL )
    delete mCapture;

  mCapture = windowCapture;
}

void Hearthstone::EnableLogging() {
  const int   NUM_INFO_MODULES = 4;
  const char  INFO_MODULES[ NUM_INFO_MODULES ][ 32 ] = { "Zone", "Asset", "Bob", "Power" };

  string path = LogConfigPath();
  QFile file( path.c_str() );

  SetRestartRequired( false );

  // Read file contents
  QString contents;
  if( file.exists() ) {
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    QTextStream in( &file );
    contents = in.readAll();
    file.close();
  }

  if( !file.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) ) {
    ERROR( "Couldn't create file %s", path.c_str() );
  } else {
    QTextStream out( &file );
    for( int i = 0; i < NUM_INFO_MODULES; i++ ) {
      const char *logModuleName = INFO_MODULES[ i ];
      if( !contents.contains( QString( "[%1]" ).arg( logModuleName ) ) ) {
        out << "\n";
        out << "[" << INFO_MODULES[i] << "]\n";
        out << "LogLevel=1\n";
        out << "ConsolePrinting=true\n";
        INFO( "Enable Log Module %s", logModuleName );

        if( Running() ) {
          SetRestartRequired( true );
        }
      }
    }
    file.close();
  }
}

void Hearthstone::DisableLogging() {
  QFile file( LogConfigPath().c_str() );
  if( file.exists() ) {
    file.remove();
    INFO( "Ingame log deactivated." );
  }
}

string Hearthstone::LogConfigPath() {
#ifdef Q_OS_MAC
  QString homeLocation = QStandardPaths::standardLocations( QStandardPaths::HomeLocation ).first();
  QString configPath = homeLocation + "/Library/Preferences/Blizzard/Hearthstone/log.config";
#elif defined Q_OS_WIN
  char buffer[ MAX_PATH ];
  SHGetSpecialFolderPathA( NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE );
  QString localAppData( buffer );
  QString configPath = localAppData + "\\Blizzard\\Hearthstone\\log.config";
#endif
  return configPath.toStdString();
}

string Hearthstone::LogPath() {
#ifdef Q_OS_MAC
  QString homeLocation = QStandardPaths::standardLocations( QStandardPaths::HomeLocation ).first();
  QString logPath = homeLocation + "/Library/Logs/Unity/Player.log";
#elif defined Q_OS_WIN
  QSettings hsKey( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hearthstone", QSettings::NativeFormat );
  QString hsPath = hsKey.value( "InstallLocation" ).toString();
  if( hsPath.isEmpty() ) {
    INFO( "LogPath Fallback" );
    QString programFiles( getenv( "PROGRAMFILES(X86)" ) );
    if( programFiles.isEmpty() ) {
      programFiles = getenv( "PROGRAMFILES" );
    }
    hsPath = programFiles + "\\Hearthstone";
  }
  QString logPath = hsPath + "\\Hearthstone_Data\\output_log.txt";
#endif
  return logPath.toStdString();
}

int Hearthstone::Width() {
  return mCapture->Width();
}

int Hearthstone::Height() {
  return mCapture->Height();
}

void Hearthstone::SetRestartRequired( bool restartRequired ) {
  mRestartRequired = restartRequired;
}

bool Hearthstone::RestartRequired() const {
  return mRestartRequired;
}
