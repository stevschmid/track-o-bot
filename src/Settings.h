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
  void ReplaysEnabledChanged( bool enabled );

  void OpenProfileRequested();

public:

  QString AccountUsername() const;
  QString AccountPassword() const;
  bool HasAccount() const;
  void SetAccount( const QString& username, const QString& password );
  void OpenProfile();

  QString WebserviceURL() const;
  void SetWebserviceURL( const QString& webserviceUrl );

  bool Autostart() const;
  void SetAutostart( bool enabled );

  bool AutoUpdateCheck() const;
  void SetAutoUpdateCheck( bool enabled );
  void CheckForUpdates();

  bool ReplayRequirementsFulfilled() const;
  bool ReplaysEnabled() const;
  void SetReplaysEnabled( bool enabled );
};
