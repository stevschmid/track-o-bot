#pragma once

#include <QString>

class Settings : public QObject
{
  Q_OBJECT

  DEFINE_SINGLETON( Settings );

signals:
  void AccountChanged( const QString& username, const QString& password );
  void WebserviceURLChanged( const QString& webserviceUrl );
  void AutostartChanged( bool enabled );
  void AutoUpdateCheckChanged( bool enabled );
  void DebugEnabledChanged( bool enabled );
  void HearthstoneDirectoryPathChanged( const QString& path );
  void OverlayEnabledChanged( bool enabled );

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

  bool DebugEnabled() const;
  void SetDebugEnabled( bool enabled );

  bool OverlayEnabled() const;
  void SetOverlayEnabled( bool enabled );

  QString HearthstoneDirectoryPath() const;
  void SetHearthstoneDirectoryPath( const QString& path );
};
