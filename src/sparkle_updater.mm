#include "sparkle_updater.h"

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

class SparkleUpdater::Private
{
  public:
    SUUpdater* updater;
};

SparkleUpdater::SparkleUpdater(const QString& aUrl)
{
  d = new Private;

  d->updater = [SUUpdater sharedUpdater];
  [d->updater retain];

  NSURL* url = [NSURL URLWithString:
      [NSString stringWithUTF8String: aUrl.toUtf8().data()]];
  [d->updater setFeedURL: url];
}

SparkleUpdater::~SparkleUpdater()
{
  [d->updater release];
  delete d;
}

void SparkleUpdater::setAutomaticallyChecksForUpdates(bool automaticallyChecks)
{
  if(automaticallyChecks) {
    [d->updater setAutomaticallyChecksForUpdates:YES];
  } else {
    [d->updater setAutomaticallyChecksForUpdates:NO];
  }
}

bool SparkleUpdater::automaticallyChecksForUpdates()
{
  return [d->updater automaticallyChecksForUpdates] == YES;
}
