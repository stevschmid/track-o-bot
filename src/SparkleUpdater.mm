#include "SparkleUpdater.h"

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

class SparkleUpdater::Private
{
public:
  SUUpdater* updater;
};

SparkleUpdater::SparkleUpdater( const QString& url )
{
  d = new Private;

  d->updater = [SUUpdater sharedUpdater];
  [d->updater retain];

  NSURL* nsUrl = [NSURL URLWithString:
      [NSString stringWithUTF8String: url.toUtf8().data()]];
  [d->updater setFeedURL: nsUrl];
}

SparkleUpdater::~SparkleUpdater()
{
  [d->updater release];
  delete d;
}

void SparkleUpdater::CheckForUpdatesNow() 
{
  return [d->updater checkForUpdates:nil];
}

void SparkleUpdater::SetAutomaticallyChecksForUpdates( bool automaticallyChecks )
{
  if( automaticallyChecks ) {
    [d->updater setAutomaticallyChecksForUpdates:YES];
  } else {
    [d->updater setAutomaticallyChecksForUpdates:NO];
  }
}

bool SparkleUpdater::AutomaticallyChecksForUpdates()
{
  return [d->updater automaticallyChecksForUpdates] == YES;
}
