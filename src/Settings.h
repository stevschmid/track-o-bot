#pragma once

#include <QString>

class Settings : public QObject
{
  Q_OBJECT

  DEFINE_SINGLETON( Settings )

signals:
  void AccountChanged( const QString& username, const QString& password );
  void WebserviceURLChanged( const QString& webserviceUrl );
  void AutostartChanged( bool enabled );
  void AutoUpdateCheckChanged( bool enabled );
  void UploadMetadataEnabledChanged( bool enabled );

public:
  QString AccountUsername() const;
  QString AccountPassword() const;
  bool HasAccount() const;
  void SetAccount( const QString& username, const QString& password );

  QString WebserviceURL() const;
  void SetWebserviceURL( const QString& webserviceUrl );

  bool Autostart() const;
  void SetAutostart( bool enabled );

  bool AutoUpdateCheck() const;
  void SetAutoUpdateCheck( bool enabled );
  void CheckForUpdates();

  bool UploadMetadataEnabled() const;
  void SetUploadMetadataEnabled( bool enabled );
};
