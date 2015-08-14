#include "ResultQueue.h"
#include "Tracker.h"

ResultQueue::ResultQueue() {
  connect( Tracker::Instance(), SIGNAL( UploadResultFailed(const Result&) ), this, SLOT( UploadResultFailed(const Result&) ) );
}

ResultQueue::~ResultQueue() {
}

void ResultQueue::Add( const Result& result ) {
  Tracker::Instance()->UploadResult( result );
}

void ResultQueue::UploadResultFailed( const Result& result ) {
}
