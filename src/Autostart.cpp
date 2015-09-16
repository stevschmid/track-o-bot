#include "Autostart.h"

#include <QApplication>
#include <QDir>
#include <QString>
#include <QSettings>
#include <QFileInfo>

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

LSSharedFileListItemRef FindLoginItemForCurrentBundle(CFArrayRef currentLoginItems)
{
  CFURLRef mainBundleURL = CFBundleCopyBundleURL( CFBundleGetMainBundle() );

  for( int i = 0, end = CFArrayGetCount( currentLoginItems ); i < end; ++i ) {
    LSSharedFileListItemRef item = ( LSSharedFileListItemRef )CFArrayGetValueAtIndex( currentLoginItems, i );

    UInt32 resolutionFlags = kLSSharedFileListNoUserInteraction | kLSSharedFileListDoNotMountVolumes;
    CFURLRef url = NULL;
    OSStatus err = LSSharedFileListItemResolve( item, resolutionFlags, &url, NULL );

    if( err == noErr ) {
      bool foundIt = CFEqual( url, mainBundleURL );
      CFRelease( url );

      if( foundIt ) {
        CFRelease( mainBundleURL );
        return item;
      }
    }
  }

  CFRelease( mainBundleURL );
  return NULL;
}

#endif // Q_OS_MAC

bool Autostart::Active()
{
#ifdef Q_OS_WIN
  const QString& applicationName = qApp->applicationName();
  QSettings tmpSettings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
  return !tmpSettings.value( applicationName ).toString().isEmpty();
#elif defined(Q_OS_MAC)

  LSSharedFileListRef loginItems = LSSharedFileListCreate( NULL, kLSSharedFileListSessionLoginItems, NULL );

  if( !loginItems ) {
    return false;
  }

  UInt32 seed = 0U;
  CFArrayRef currentLoginItems = LSSharedFileListCopySnapshot( loginItems, &seed );
  LSSharedFileListItemRef existingItem = FindLoginItemForCurrentBundle( currentLoginItems );

  bool isAutoRun = existingItem != NULL;

  CFRelease( currentLoginItems );
  CFRelease( loginItems );

  return isAutoRun;
#endif
}

void Autostart::SetActive( bool active )
{
#ifdef Q_OS_WIN
  const QString& applicationName = qApp->applicationName();

  QSettings tmpSettings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
  if( active ) {
    tmpSettings.setValue( applicationName,
      QString( "\"%1\"" ).arg( QDir::toNativeSeparators( QFileInfo( qApp->arguments()[0] ).filePath() ) )
    );
  } else {
    tmpSettings.remove(applicationName);
  }

#elif defined(Q_OS_MAC)
  LSSharedFileListRef loginItems = LSSharedFileListCreate( NULL, kLSSharedFileListSessionLoginItems, NULL );

  if( !loginItems )
    return;

  UInt32 seed = 0U;
  CFArrayRef currentLoginItems = LSSharedFileListCopySnapshot( loginItems, &seed );
  LSSharedFileListItemRef existingItem = FindLoginItemForCurrentBundle( currentLoginItems );

  if( active && (existingItem == NULL) ) {
    CFURLRef mainBundleURL = CFBundleCopyBundleURL( CFBundleGetMainBundle() );
    LSSharedFileListInsertItemURL( loginItems, kLSSharedFileListItemBeforeFirst, NULL, NULL, mainBundleURL, NULL, NULL );
    CFRelease( mainBundleURL );
  }
  else if( !active && (existingItem != NULL) ) {
    LSSharedFileListItemRemove(loginItems, existingItem);
  }

  CFRelease( currentLoginItems );
  CFRelease( loginItems );
#endif
}
