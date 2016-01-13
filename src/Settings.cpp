#include "Settings.h"

#include <QSettings>

#include "Autostart.h"

#include "Updater.h"
extern Updater *gUpdater;

DEFINE_SINGLETON_SCOPE( Settings );

#define KEY_ACCOUNT_USERNAME "username"
#define KEY_ACCOUNT_PASSWORD "password"
#define KEY_WEBSERVICE_URL "webserviceUrl"
#define KEY_UPLOAD_METADATA_ENABLED "uploadMetadataEnabled"

Settings::Settings() {
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

bool Settings::UploadMetadataEnabled() const {
  return QSettings().value( KEY_UPLOAD_METADATA_ENABLED, false ).toBool();
}

void Settings::SetUploadMetadataEnabled( bool enabled ) {
  QSettings().setValue( KEY_UPLOAD_METADATA_ENABLED, enabled );

  emit UploadMetadataEnabledChanged( enabled );
}
