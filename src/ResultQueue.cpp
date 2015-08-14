#include "ResultQueue.h"
#include "Tracker.h"

ResultQueue::ResultQueue() {
  connect( Tracker::Instance(), SIGNAL( UploadResultFailed(const Result&) ), this, SLOT( UploadResultFailed(const Result&) ) );
}

ResultQueue::~ResultQueue() {
}

void ResultQueue::Add( const Result& res ) {
  if( res.mode == MODE_SOLO_ADVENTURES ) {
    LOG( "Ignore solo adventure" );
    return;
  }

  if( res.mode == MODE_TAVERN_BRAWL ) {
    LOG( "Ignore tavern brawl" );
    return;
  }

  if( res.outcome == OUTCOME_UNKNOWN ) {
    LOG( "Outcome unknown. Skip result" );
    return;
  }

  if( res.mode == MODE_UNKNOWN ) {
    LOG( "Mode unknown. Skip result" );
    return;
  }

  if( res.order == ORDER_UNKNOWN ) {
    LOG( "Order unknown. Skip result" );
    return;
  }

  if( res.hero == CLASS_UNKNOWN ) {
    LOG( "Own Class unknown. Skip result" );
    return;
  }

  if( res.opponent == CLASS_UNKNOWN ) {
    LOG( "Class of Opponent unknown. Skip result" );
    return;
  }

  Tracker::Instance()->UploadResult( res );
}

void ResultQueue::UploadResultFailed( const Result& result ) {
}
