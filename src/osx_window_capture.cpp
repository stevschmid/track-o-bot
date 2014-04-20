#include "osx_window_capture.h"

// remove the window title bar which we are not interested in
#define OSX_WINDOW_TITLE_BAR_HEIGHT 22

OSXWindowCapture::OSXWindowCapture(const string& windowName)
  :name(windowName)
{
  Update();
}

void OSXWindowCapture::Update() {
  winId = FindWindow(name);
  if( winId == 0 )
    return;

  rect = GetWindowRect(winId);
}

int OSXWindowCapture::GetWidth()
{
  return CGRectGetWidth(rect);
}

int OSXWindowCapture::GetHeight()
{
  int height = CGRectGetHeight(rect);
  return IsFullscreen() ? height : height - OSX_WINDOW_TITLE_BAR_HEIGHT;
}

QPixmap OSXWindowCapture::Capture(int x, int y, int w, int h)
{
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

  QPixmap pixmap = QPixmap::fromMacCGImageRef(image);

  CGImageRelease(image);

  return pixmap;
}

int OSXWindowCapture::FindWindow(const string& name) {
  int winId = 0;

  CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements, kCGNullWindowID);
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

  return winId;
}

CGRect OSXWindowCapture::GetWindowRect(int windowId) {
  CGRect rect;

  CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow, windowId);
  CFIndex numWindows = CFArrayGetCount(windowList);
  if( numWindows > 0 ) {
    CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, 0);
    CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)CFDictionaryGetValue(info, kCGWindowBounds), &rect);
  }

  return rect;
}

bool OSXWindowCapture::IsFullscreen() {
  // this is not the most elegant solution, but I couldn't find a better way
  return CGRectGetMinX(rect) == 0.0f &&
    CGRectGetMinY(rect) == 0.0f &&
    (int(CGRectGetHeight(rect)) & OSX_WINDOW_TITLE_BAR_HEIGHT) != OSX_WINDOW_TITLE_BAR_HEIGHT;
}
