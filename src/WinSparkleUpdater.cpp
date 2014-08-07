#include "WinSparkleUpdater.h"

#include <winsparkle.h>
#include <QApplication>

#define WIDEN(str) WIDEN_(str)
#define WIDEN_(str) L##str

WinSparkleUpdater::WinSparkleUpdater( const QString& url )
{
  win_sparkle_set_appcast_url( url.toUtf8().data() );
  win_sparkle_set_app_details(
    qApp->organizationName().toStdWString().c_str(),
    qApp->applicationName().toStdWString().c_str(),
    WIDEN(VERSION)
  );

  win_sparkle_init();
}

WinSparkleUpdater::~WinSparkleUpdater()
{
  win_sparkle_cleanup();
}

void WinSparkleUpdater::CheckForUpdatesNow()
{
  win_sparkle_check_update_with_ui();
}

void WinSparkleUpdater::SetAutomaticallyChecksForUpdates( bool automaticallyChecks )
{
  win_sparkle_set_automatic_check_for_updates(automaticallyChecks);
}

bool WinSparkleUpdater::AutomaticallyChecksForUpdates()
{
  return win_sparkle_get_automatic_check_for_updates();
}
