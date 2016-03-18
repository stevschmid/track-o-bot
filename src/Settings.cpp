#include "Settings.h"

#include <QSettings>

#include "Autostart.h"
#include "Hearthstone.h"

#include "Updater.h"
extern Updater *gUpdater;

DEFINE_SINGLETON_SCOPE( Settings );

#define KEY_ACCOUNT_USERNAME "username"
#define KEY_ACCOUNT_PASSWORD "password"
#define KEY_WEBSERVICE_URL "webserviceUrl"
#define KEY_DEBUG_ENABLED "debug"
#define KEY_HEARTHSTONE_DIRECTORY_PATH "hearthstoneDirectoryPath"
#define KEY_OVERLAY_ENABLED "overlayEnabled"

Settings::Settings() {
  // Enable overlay by default for new users, but not for existing ones
  if( !QSettings().contains( KEY_OVERLAY_ENABLED ) ) {
    bool isNewUser = !HasAccount();
    SetOverlayEnabled( isNewUser );
  }
}

Settings::~Settings() {
}


QString Settings::AccountUsername() const {
  return QSettings().value( KEY_ACCOUNT_USERNAME ).toString();
}

QString Settings::AccountPassword() const {
  return QSettings().value( KEY_ACCOUNT_PASSWORD ).toString();
}

bool Settings::HasAccount() const {
  return !AccountUsername().isEmpty() && !AccountPassword().isEmpty();
}

void Settings::SetAccount( const QString& username, const QString& password ) {
  QSettings s;
  s.setValue( KEY_ACCOUNT_USERNAME, username );
  s.setValue( KEY_ACCOUNT_PASSWORD, password );

  emit AccountChanged( username, password );
}

QString Settings::WebserviceURL() const {
  return QSettings().value( KEY_WEBSERVICE_URL ).toString();
}

void Settings::SetWebserviceURL( const QString& webserviceUrl ) {
  QSettings s;
  s.setValue( KEY_WEBSERVICE_URL, webserviceUrl );

  emit WebserviceURLChanged( webserviceUrl );
}

bool Settings::Autostart() const {
  return ::Autostart().Active();
}

void Settings::SetAutostart( bool enabled ) {
  ::Autostart().SetActive( enabled );

  emit AutostartChanged( enabled );
}

bool Settings::AutoUpdateCheck() const {
  bool enabled = gUpdater && gUpdater->AutomaticallyChecksForUpdates();
  return enabled;
}

void Settings::SetAutoUpdateCheck( bool enabled ) {
  if( gUpdater ) {
    gUpdater->SetAutomaticallyChecksForUpdates( enabled );
  }

  emit AutoUpdateCheckChanged( enabled );
}

void Settings::CheckForUpdates() {
  if( gUpdater ) {
    gUpdater->CheckForUpdatesNow();
  }
}

bool Settings::DebugEnabled() const {
  return QSettings().value( KEY_DEBUG_ENABLED, false ).toBool();
}

void Settings::SetDebugEnabled( bool enabled ) {
  QSettings().setValue( KEY_DEBUG_ENABLED, enabled );

  emit DebugEnabledChanged( enabled );
}

bool Settings::OverlayEnabled() const {
  return QSettings().value( KEY_OVERLAY_ENABLED ).toBool();
}

void Settings::SetOverlayEnabled( bool enabled ) {
  QSettings().setValue( KEY_OVERLAY_ENABLED, enabled );
  emit OverlayEnabledChanged( enabled );
}

QString Settings::HearthstoneDirectoryPath() const {
  QString path = QSettings().value( KEY_HEARTHSTONE_DIRECTORY_PATH ).toString();
  if( path.isEmpty() ) {
    path = Hearthstone::Instance()->DetectHearthstonePath();
  }
  return path;
}

void Settings::SetHearthstoneDirectoryPath( const QString& path ) {
  QSettings s;
  s.setValue( KEY_HEARTHSTONE_DIRECTORY_PATH, path );

  emit HearthstoneDirectoryPathChanged( path );
}
