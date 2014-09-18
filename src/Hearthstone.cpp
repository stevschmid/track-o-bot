#include "Hearthstone.h"

#include <QFile>
#include <QDesktopServices>
#include <QSettings>

#ifdef Q_WS_MAC
#include "OSXWindowCapture.h"
#elif defined Q_WS_WIN
#include "WinWindowCapture.h"
#include "Shlobj.h"
#elif defined Q_WS_X11
#include <math.h>
#include "LinuxWindowCapture.h"
#endif

DEFINE_SINGLETON_SCOPE( Hearthstone );

Hearthstone::Hearthstone()
 : mRestartRequired( false )
{
#ifdef Q_WS_MAC
  mCapture = new OSXWindowCapture( "Hearthstone" );
#elif defined Q_WS_WIN
  mCapture = new WinWindowCapture( "Hearthstone" );
#elif defined Q_WS_X11
 mCapture = new LinuxWindowCapture ( "Hearthstone" );
#endif
}

Hearthstone::~Hearthstone() {
  if( mCapture != NULL )
    delete mCapture;
}

bool Hearthstone::Running() {
  return mCapture->WindowFound();
}

#ifdef Q_WS_WIN
inline float roundf( float x ) {
   return x >= 0.0f ? floorf( x + 0.5f ) : ceilf( x - 0.5f );
}
#endif

QPixmap Hearthstone::Capture( int vx, int vy, int vw, int vh ) {
  int x, y, w, h;

  int realCanvasWidth = mCapture->Width();
  int realCanvasHeight = mCapture->Height();

  int virtualCanvasWidth = VIRTUAL_CANVAS_WIDTH;
  int virtualCanvasHeight = VIRTUAL_CANVAS_HEIGHT;

  float dx = ( vx - virtualCanvasWidth/2 );
  float dy = ( vy - virtualCanvasHeight/2 );
  float scale = ( float )realCanvasHeight / virtualCanvasHeight;

  // Rounding here is important for dhash calc
  x = roundf( realCanvasWidth/2 + dx * scale );
  y = roundf( realCanvasHeight/2 + dy * scale );
  w = roundf( vw * scale );
  h = roundf( vh * scale );

  return mCapture->Capture( x, y, w, h );
}

void Hearthstone::SetWindowCapture( WindowCapture *windowCapture ) {
  if( mCapture != NULL )
    delete mCapture;

  mCapture = windowCapture;
}

void Hearthstone::EnableLogging() {
  const int   NUM_LOG_MODULES = 4;
  const char  LOG_MODULES[ NUM_LOG_MODULES ][ 32 ] = { "Zone", "Asset", "Bob", "Power" };

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
    LOG( "Couldn't create file %s", path.c_str() );
  } else {
    QTextStream out( &file );
    for( int i = 0; i < NUM_LOG_MODULES; i++ ) {
      const char *logModuleName = LOG_MODULES[ i ];
      if( !contents.contains( QString( "[%1]" ).arg( logModuleName ) ) ) {
        out << "\n";
        out << "[" << LOG_MODULES[i] << "]\n";
        out << "LogLevel=1\n";
        out << "ConsolePrinting=true\n";
        LOG( "Enable Log Module %s", logModuleName );

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
    LOG( "Ingame log deactivated." );
  }
}

string Hearthstone::LogConfigPath() {
#ifdef Q_WS_MAC
  QString homeLocation = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
  QString configPath = homeLocation + "/Library/Preferences/Blizzard/Hearthstone/log.config";
#elif defined Q_WS_WIN
  char buffer[ MAX_PATH ];
  SHGetSpecialFolderPathA( NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE );
  QString localAppData( buffer );
  QString configPath = localAppData + "\\Blizzard\\Hearthstone\\log.config";
#elif defined Q_WS_X11
  QString homeLocation = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
  QString configPath = homeLocation + "/.Hearthstone/log.config";
  LOG("HS config file: %s", configPath.toStdString().c_str());
#endif
  return configPath.toStdString();
}

string Hearthstone::LogPath() {
#ifdef Q_WS_MAC
  QString homeLocation = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
  QString logPath = homeLocation + "/Library/Logs/Unity/Player.log";
#elif defined Q_WS_WIN
  QSettings hsKey( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hearthstone", QSettings::NativeFormat );
  QString hsPath = hsKey.value( "InstallLocation" ).toString();
  if( hsPath.isEmpty() ) {
    LOG( "LogPath Fallback" );
    QString programFiles( getenv( "PROGRAMFILES(X86)" ) );
    if( programFiles.isEmpty() ) {
      programFiles = getenv( "PROGRAMFILES" );
    }
    hsPath = programFiles + "\\Hearthstone";
  }
  QString logPath = hsPath + "\\Hearthstone_Data\\output_log.txt";
#elif defined Q_WS_X11
  QString homeLocation = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
  QString logPath = homeLocation + "/.Hearthstone/output_log.txt";
#endif
  LOG("HS log file: %s", logPath.toStdString().c_str());
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
