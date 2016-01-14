#include "ResultTracker.h"
#include "Hearthstone.h"

#include <map>

ResultTracker::ResultTracker()
  : mSpectating( false ), mCurrentGameMode( MODE_UNKNOWN )
{
  connect( &mLogTracker, &HearthstoneLogTracker::HandleOutcome, this, &ResultTracker::HandleOutcome );
  connect( &mLogTracker, &HearthstoneLogTracker::HandleOrder, this, &ResultTracker::HandleOrder );
  connect( &mLogTracker, &HearthstoneLogTracker::HandleOwnClass, this, &ResultTracker::HandleOwnClass ) ;
  connect( &mLogTracker, &HearthstoneLogTracker::HandleOpponentClass, this, &ResultTracker::HandleOpponentClass );
  connect( &mLogTracker, &HearthstoneLogTracker::HandleGameMode, this, &ResultTracker::HandleGameMode );
  connect( &mLogTracker, &HearthstoneLogTracker::HandleLegend, this, &ResultTracker::HandleLegend );
  connect( &mLogTracker, &HearthstoneLogTracker::HandleTurn, this, &ResultTracker::HandleTurn );

  connect( &mLogTracker, &HearthstoneLogTracker::HandleSpectating, this, &ResultTracker::HandleSpectating );
  connect( &mLogTracker, &HearthstoneLogTracker::HandleMatchStart, this, &ResultTracker::HandleMatchStart );
  connect( &mLogTracker, &HearthstoneLogTracker::HandleMatchEnd, this, &ResultTracker::HandleMatchEnd );

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
  mResult.mode = mCurrentGameMode;
  UploadResult();
}

void ResultTracker::HandleGameMode( GameMode mode ) {
  DBG( "HandleGameMode %s", MODE_NAMES[ mode ] );
  mCurrentGameMode = mode;
}

void ResultTracker::HandleLegend( int legend ) {
  DBG( "Set Legend %d", legend );
  mResult.legend = legend;
}

void ResultTracker::HandleTurn( int turn ) {
  UNUSED_ARG( turn );

  if( turn > 1 ) { // turn 1 (first player) happens before game is in-effect [mulligan]
    QImage label;
    float score;

    int rank = mRankClassifier.DetectCurrentRank( &score, &label );
    mRanks.push_back( rank );
    DBG( "Turn %d. Set Rank %d", turn, rank );

    METADATA( QString( "RANK_CLASSIFIER_%1_RANK" ).arg( turn ), rank );
    METADATA( QString( "RANK_CLASSIFIER_%1_SCORE" ).arg( turn ), score );
    METADATA( QString( "RANK_CLASSIFIER_%1_LABEL" ).arg( turn ), label );
  }
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
