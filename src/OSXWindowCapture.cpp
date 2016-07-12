#include "OSXWindowCapture.h"

#include <QtMac>

// remove the window title bar which we are not interested in
#define OSX_WINDOW_TITLE_BAR_HEIGHT 22

OSXWindowCapture::OSXWindowCapture()
  : mWinId( 0 ), mFocus( false )
{
}

bool OSXWindowCapture::WindowFound() {
  if( mWinId == 0 ) {
    mWinId = FindWindow( "Hearthstone" );
  }

  if( mWinId && !ExtractWindowProperties( mWinId, &mRect, &mFocus ) ) {
    // Window became invalid
    mWinId = 0;
  }

  return mWinId != 0;
}

int OSXWindowCapture::Width() {
  return CGRectGetWidth( mRect );
}

int OSXWindowCapture::Height() {
  int height = CGRectGetHeight( mRect );
  return Fullscreen() ? height : std::max< int >( height - OSX_WINDOW_TITLE_BAR_HEIGHT, 0 );
}

int OSXWindowCapture::Left() {
  return CGRectGetMinX( mRect );
}

int OSXWindowCapture::Top() {
  return CGRectGetMinY( mRect ) + ( Fullscreen() ? 0 : OSX_WINDOW_TITLE_BAR_HEIGHT );
}

QPixmap OSXWindowCapture::Capture( int x, int y, int w, int h ) {
  CGRect captureRect = CGRectMake(
      x + Left(),
      y + Top(),
      w,
      h );

  CGImageRef image = CGWindowListCreateImage( captureRect,
      kCGWindowListOptionIncludingWindow,
      mWinId,
      kCGWindowImageNominalResolution | kCGWindowImageBoundsIgnoreFraming );

  QPixmap pixmap = QtMac::fromCGImageRef( image );
  CGImageRelease( image );

  return pixmap;
}

bool OSXWindowCapture::Fullscreen() {
  // this is not the most elegant solution, but I couldn't find a better way
  return CGRectGetMinX( mRect ) == 0.0f && CGRectGetMinY( mRect ) == 0.0f &&
    ( int( CGRectGetHeight( mRect ) ) & OSX_WINDOW_TITLE_BAR_HEIGHT ) != OSX_WINDOW_TITLE_BAR_HEIGHT;
}

int OSXWindowCapture::FindWindow( const QString& name ) {
  int winId = 0;

  CFArrayRef windowList = CGWindowListCopyWindowInfo(
      kCGWindowListExcludeDesktopElements,
      kCGNullWindowID );
  CFIndex numWindows = CFArrayGetCount( windowList );

  CFStringRef nameRef = name.toCFString();

  for( int i = 0; i < (int)numWindows; i++ ) {
    CFDictionaryRef info = ( CFDictionaryRef )CFArrayGetValueAtIndex( windowList, i );
    CFStringRef thisWindowName = ( CFStringRef )CFDictionaryGetValue( info, kCGWindowName );
    CFStringRef thisWindowOwnerName = ( CFStringRef )CFDictionaryGetValue( info, kCGWindowOwnerName );

    if( thisWindowOwnerName && CFStringCompare( thisWindowOwnerName, nameRef, 0 ) == kCFCompareEqualTo ) {
      if( thisWindowName && CFStringCompare( thisWindowName, nameRef, 0 ) == kCFCompareEqualTo ) {
        CFNumberRef thisWindowNumber = ( CFNumberRef )CFDictionaryGetValue( info, kCGWindowNumber );
        CFNumberGetValue( thisWindowNumber, kCFNumberIntType, &winId );
        break;
      }
    }
  }

  CFRelease( nameRef );
  CFRelease( windowList );

  return winId;
}

bool OSXWindowCapture::ExtractWindowProperties( int windowId, CGRect *rect, bool *focus ) {
  CFArrayRef windowList = CGWindowListCopyWindowInfo( kCGWindowListOptionIncludingWindow, windowId );
  CFIndex numWindows = CFArrayGetCount( windowList );

  *focus = false;

  if( numWindows > 0 ) {
    CFDictionaryRef info = ( CFDictionaryRef )CFArrayGetValueAtIndex( windowList, 0 );
    CGRectMakeWithDictionaryRepresentation( ( CFDictionaryRef )CFDictionaryGetValue( info, kCGWindowBounds ), rect);
    CFBooleanRef thisWindowOnScreen = ( CFBooleanRef )( CFDictionaryGetValue( info, kCGWindowIsOnscreen ) );
    if( thisWindowOnScreen != NULL ) {
      *focus = CFBooleanGetValue( thisWindowOnScreen );
    }
  }

  CFRelease( windowList );
  return numWindows > 0;
}

bool OSXWindowCapture::HasFocus() {
  return mFocus;
}
