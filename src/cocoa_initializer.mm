#include "cocoa_initializer.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

class CocoaInitializer::Private 
{
  public:
    NSAutoreleasePool* autoReleasePool_;
};

CocoaInitializer::CocoaInitializer()
{
  d = new CocoaInitializer::Private();
  NSApplicationLoad();
  d->autoReleasePool_ = [[NSAutoreleasePool alloc] init];
}

CocoaInitializer::~CocoaInitializer()
{
  [d->autoReleasePool_ release];
  delete d;
}
