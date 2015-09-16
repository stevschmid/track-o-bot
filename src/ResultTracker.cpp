#include "ResultTracker.h"
#include "Hearthstone.h"

#include <map>

ResultTracker::ResultTracker()
  : mSpectating( false )
{
  connect( &mLogTracker, SIGNAL( HandleOutcome(Outcome) ), this, SLOT( HandleOutcome(Outcome) ) );
  connect( &mLogTracker, SIGNAL( HandleOrder(GoingOrder) ), this, SLOT( HandleOrder(GoingOrder) ) );
  connect( &mLogTracker, SIGNAL( HandleOwnClass(Class) ), this, SLOT( HandleOwnClass(Class) ) ) ;
  connect( &mLogTracker, SIGNAL( HandleOpponentClass(Class) ), this, SLOT( HandleOpponentClass(Class) ) );
  connect( &mLogTracker, SIGNAL( HandleGameMode(GameMode) ), this, SLOT( HandleGameMode(GameMode) ) );
  connect( &mLogTracker, SIGNAL( HandleLegend(int) ), this, SLOT( HandleLegend(int) ) );
  connect( &mLogTracker, SIGNAL( HandleTurn(int) ), this, SLOT( HandleTurn(int) ) );

  connect( &mLogTracker, SIGNAL( HandleSpectating(bool) ), this, SLOT( HandleSpectating(bool) ) );
  connect( &mLogTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( &mLogTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&) ), this, SLOT( HandleMatchEnd(const ::CardHistoryList&) ) );

  ResetResult();
}

ResultTracker::~ResultTracker() {
}

void ResultTracker::ResetResult() {
  mResult.Reset();
  mRanks.clear();
  mSpectating = false;
}

void ResultTracker::HandleOrder( GoingOrder order ) {
  DBG( "HandleOrder %s", ORDER_NAMES[ order ] );
  mResult.order = order;
}

void ResultTracker::HandleOutcome( Outcome outcome ) {
  DBG( "HandleOutcome %s", OUTCOME_NAMES[ outcome ] );
  mResult.outcome = outcome;
}

void ResultTracker::HandleOwnClass( Class ownClass ) {
  DBG( "HandleOwnClass %s", CLASS_NAMES[ ownClass ] );
  mResult.hero = ownClass;
}

void ResultTracker::HandleOpponentClass( Class opponentClass ) {
  DBG( "HandleOpponentClass %s", CLASS_NAMES[ opponentClass ] );
  mResult.opponent = opponentClass;
}

void ResultTracker::HandleMatchStart() {
  DBG( "HandleMatchStart" );
  mDurationTimer.start();
}

void ResultTracker::HandleSpectating( bool nowSpectating ) {
  DBG( "HandleSpectating" );
  mSpectating = nowSpectating;
}

void ResultTracker::HandleMatchEnd( const ::CardHistoryList& cardHistoryList ) {
  if( mSpectating ) {
    LOG( "Ignore spectated match" );
    ResetResult();
    return;
  }

  DBG( "HandleMatchEnd" );
  mResult.cardList = cardHistoryList;
  mResult.duration = mDurationTimer.elapsed() / 1000;
  UploadResult();
}

void ResultTracker::HandleGameMode( GameMode mode ) {
  DBG( "HandleGameMode %s", MODE_NAMES[ mode ] );
  mResult.mode = mode;
}

void ResultTracker::HandleLegend( int legend ) {
  DBG( "Set Legend %d", legend );
  mResult.legend = legend;
}

void ResultTracker::HandleTurn( int turn ) {
  UNUSED_ARG( turn );

  int rank = mRankClassifier.DetectCurrentRank();
  mRanks.push_back( rank );
  DBG( "Turn %d. Set Rank %d", turn, rank );
}

// Screen capture can be tricky
// So capture the rank a few times during the game
// and the majority vote will be the determined rank
int ResultTracker::DetermineRank() {
  std::map< int, int > votesByRank;

  int maxVote = 0;
  int maxRank = RANK_UNKNOWN;

  for( int rank : mRanks ) {
    votesByRank[ rank ] += 1;
  }

  for( std::map<int,int>::iterator it = votesByRank.begin(); it != votesByRank.end(); ++it ) {
    DBG( "Rank %d has %d votes", (*it).first, (*it).second );
    if( (*it).second > maxVote ) {
      maxVote = (*it).second;
      maxRank = (*it).first;
    }
  }

  return maxRank;
}

void ResultTracker::UploadResult() {
  DBG( "UploadResult" );

  mResult.rank = DetermineRank();
  DBG( "Determined Rank: %d", mResult.rank );

  mResultsQueue.Add( mResult );
  ResetResult();
}
