#include "Hearthstone.h"

#include <QFile>
#include <QDesktopServices>
#include <QSettings>

#ifdef Q_WS_MAC
#include "OSXWindowCapture.h"
#elif defined Q_WS_WIN
#include "WinWindowCapture.h"
#include "Shlobj.h"
#endif

DEFINE_SINGLETON_SCOPE( Hearthstone )

Hearthstone::Hearthstone()
 : mRestartRequired( false )
{
#ifdef Q_WS_MAC
  mCapture = new OSXWindowCapture( "Hearthstone" );
#elif defined Q_WS_WIN
  mCapture = new WinWindowCapture( "Hearthstone" );
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
  const int   NUM_LOG_MODULES = 4;
  const char  LOG_MODULES[ NUM_LOG_MODULES ][ 32 ] = { "Zone", "Asset", "Bob", "Power" };

  QString path = LogConfigPath();
  QFile file( path );

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
    LOG( "Couldn't create file %s", path.toStdString().c_str() );
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
  QFile file( LogConfigPath() );
  if( file.exists() ) {
    file.remove();
    LOG( "Ingame log deactivated." );
  }
}

QString Hearthstone::LogConfigPath() {
#ifdef Q_WS_MAC
  QString homeLocation = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
  QString configPath = homeLocation + "/Library/Preferences/Blizzard/Hearthstone/log.config";
#elif defined Q_WS_WIN
  wchar_t buffer[ MAX_PATH ];
  SHGetSpecialFolderPathW( NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE );
  QString localAppData = QString::fromWCharArray( buffer );
  QString configPath = localAppData + "\\Blizzard\\Hearthstone\\log.config";
#endif
  return configPath;
}

QString Hearthstone::LogPath() {
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
#endif
  return logPath;
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
