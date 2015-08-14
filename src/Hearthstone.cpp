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
  mCapture = new OSXWindowCapture( WindowName() );
#elif defined Q_OS_WIN
  mCapture = new WinWindowCapture( WindowName() );
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
    ERR( "Couldn't open %s (%d)", path.toStdString().c_str(), file.error() );
    return "";
  }

  QString contents = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson( contents.toUtf8() );
  QJsonObject root = doc.object();

  QStringList keys;
  keys << "/game/hs_beta" << "resource" << "game";

  QJsonObject hs;
  if( !FindJsonObject( root, keys, &hs ) ) {
    ERR( "Couldn't find HS key" );
    return "";
  }

  return hs[ QString( attributeName ) ].toString();
}

bool Hearthstone::Running() const {
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

  DBG("x %d y %d w %d h %d | ch %d wh %d", x, y, w, h, canvasHeight, windowHeight);

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
    ERR( "Couldn't create file %s", path.toStdString().c_str() );
  } else {
    QTextStream out( &file );
    for( int i = 0; i < NUM_INFO_MODULES; i++ ) {
      const char *logModuleName = INFO_MODULES[ i ];
      if( !contents.contains( QString( "[%1]" ).arg( logModuleName ) ) ) {
        out << "\n";
        out << "[" << INFO_MODULES[i] << "]\n";
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

QString Hearthstone::LogConfigPath() const {
#ifdef Q_OS_MAC
  QString homeLocation = QStandardPaths::standardLocations( QStandardPaths::HomeLocation ).first();
  QString configPath = homeLocation + "/Library/Preferences/Blizzard/Hearthstone/log.config";
#elif defined Q_OS_WIN
  wchar_t buffer[ MAX_PATH ];
  SHGetSpecialFolderPathW( NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE );
  QString localAppData = QString::fromWCharArray( buffer );
  QString configPath = localAppData + "\\Blizzard\\Hearthstone\\log.config";
#endif
  return configPath;
}

QString Hearthstone::LogPath() const {
#ifdef Q_OS_MAC
  QString homeLocation = QStandardPaths::standardLocations( QStandardPaths::HomeLocation ).first();
  QString logPath = homeLocation + "/Library/Logs/Unity/Player.log";
#elif defined Q_OS_WIN

  QString hsPath = ReadAgentAttribute( "install_dir" );
  if( hsPath.isEmpty() ) {
    LOG( "Registry fallback for path" );

    QSettings hsKey( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hearthstone", QSettings::NativeFormat );
    hsPath = hsKey.value( "InstallLocation" ).toString();
  }

  if( hsPath.isEmpty() ) {
    ERR( "Hearthstone path not found" );
    return "";
  }

  QString logPath = hsPath + "\\Hearthstone_Data\\output_log.txt";
#endif
  return logPath;
}

QString Hearthstone::WindowName() const {
  QString locale = ReadAgentAttribute( "selected_locale" );
  QString windowName = "Hearthstone";

#ifdef Q_OS_MAC
  // Under mac the window name is not localized
  return windowName;
#endif

  if( locale == "zhCN" ) {
    windowName = QString::fromWCharArray( L"炉石传说" );
  } else if( locale == "zhTW" ) {
    windowName = QString::fromWCharArray( L"《爐石戰記》" );
  } else if( locale == "koKR") {
    windowName = QString::fromWCharArray( L"하스스톤" );
  }

  QFile file("C:\\tmp\\out.txt");
  file.open(QIODevice::Text | QIODevice::WriteOnly );

  QTextStream out(&file);
  out << windowName << "\n";
  file.close();

  return windowName;
}

int Hearthstone::Width() const {
  return mCapture->Width();
}

int Hearthstone::Height() const {
  return mCapture->Height();
}

void Hearthstone::SetRestartRequired( bool restartRequired ) {
  mRestartRequired = restartRequired;
}

bool Hearthstone::RestartRequired() const {
  return mRestartRequired;
}
