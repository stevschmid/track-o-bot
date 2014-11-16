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
