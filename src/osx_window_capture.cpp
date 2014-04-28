#include "osx_window_capture.h"

// remove the window title bar which we are not interested in
#define OSX_WINDOW_TITLE_BAR_HEIGHT 22

OSXWindowCapture::OSXWindowCapture(const string& windowName)
  :name(windowName), winId(0)
{
  Update(true);
}

void OSXWindowCapture::Update(bool forced) {
  if(forced || updateTimer.hasExpired(OSX_UPDATE_WINDOW_DATA_INTERVAL)) {
    winId = FindWindow(name);
    rect = GetWindowRect(winId);

    updateTimer.restart();
  }
}

bool OSXWindowCapture::WindowFound() {
  Update();
  return winId != 0;
}

int OSXWindowCapture::GetWidth() {
  Update();

  return CGRectGetWidth(rect);
}

int OSXWindowCapture::GetHeight() {
  Update();

  int height = CGRectGetHeight(rect);
  return IsFullscreen() ? height : max<int>(height - OSX_WINDOW_TITLE_BAR_HEIGHT, 0);
}

QPixmap OSXWindowCapture::Capture(int x, int y, int w, int h) {
  Update();

  if(!w) w = GetWidth();
  if(!w) h = GetHeight();

  CGRect captureRect = CGRectMake(x + CGRectGetMinX(rect),
      y + CGRectGetMinY(rect) + (IsFullscreen() ? 0 : OSX_WINDOW_TITLE_BAR_HEIGHT),
      w,
      h);

  CGImageRef image = CGWindowListCreateImage(captureRect,
      kCGWindowListOptionIncludingWindow,
      winId,
      kCGWindowImageNominalResolution |  kCGWindowImageBoundsIgnoreFraming);

  QPixmap pixmap = QtMac::fromCGImageRef(image);

  CGImageRelease(image);

  return pixmap;
}

bool OSXWindowCapture::IsFullscreen() {
  // this is not the most elegant solution, but I couldn't find a better way
  return CGRectGetMinX(rect) == 0.0f &&
    CGRectGetMinY(rect) == 0.0f &&
    (int(CGRectGetHeight(rect)) & OSX_WINDOW_TITLE_BAR_HEIGHT) != OSX_WINDOW_TITLE_BAR_HEIGHT;
}

int OSXWindowCapture::FindWindow(const string& name) {
  int winId = 0;

  CFArrayRef windowList = CGWindowListCopyWindowInfo(
      kCGWindowListExcludeDesktopElements | kCGWindowListOptionOnScreenOnly,
      kCGNullWindowID);
  CFIndex numWindows = CFArrayGetCount(windowList);

  CFStringRef nameRef = CFStringCreateWithCString(kCFAllocatorDefault, name.c_str(), kCFStringEncodingMacRoman);

  for( int i = 0; i < (int)numWindows; i++ ) {
    CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
    CFStringRef thisWindowName = (CFStringRef)CFDictionaryGetValue(info, kCGWindowName);
    CFStringRef thisWindowOwnerName = (CFStringRef)CFDictionaryGetValue(info, kCGWindowOwnerName);
    CFNumberRef thisWindowNumber = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowNumber);

    if(thisWindowOwnerName && CFStringCompare(thisWindowOwnerName, nameRef, 0) == kCFCompareEqualTo) {
      if(thisWindowName && CFStringCompare(thisWindowName, nameRef, 0) == kCFCompareEqualTo) {
        CFNumberGetValue(thisWindowNumber, kCFNumberIntType, &winId);
        break;
      }
    }
  }

  CFRelease(nameRef);
  CFRelease(windowList);

  return winId;
}

CGRect OSXWindowCapture::GetWindowRect(int windowId) {
  CGRect rect = CGRectMake(0, 0, 0, 0);

  CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow, windowId);
  CFIndex numWindows = CFArrayGetCount(windowList);
  if( numWindows > 0 ) {
    CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, 0);
    CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)CFDictionaryGetValue(info, kCGWindowBounds), &rect);
  }

  CFRelease(windowList);
  return rect;
}
