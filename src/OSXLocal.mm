#include "OSXLocal.h"
#include <Foundation/Foundation.h>

bool OSX_YosemiteDarkModeEnabled() {
  bool darkMode = false;
  NSString *interfaceStyle = [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleInterfaceStyle"];
  if( interfaceStyle ) {
    darkMode = [interfaceStyle isEqualToString: @"Dark"];
  }
  return darkMode;
}

void OSX_ShowNotification( const char *title, const char *message ) {
  NSUserNotification *notification = [[NSUserNotification alloc] init];
  notification.title = [NSString stringWithUTF8String:title];
  notification.informativeText = [NSString stringWithUTF8String:message];
  [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
}
