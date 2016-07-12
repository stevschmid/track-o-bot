#include "ResultTracker.h"
#include "Hearthstone.h"

#include <map>

ResultTracker::ResultTracker( QObject *parent )
  : QObject( parent ), mSpectating( false ), mCurrentGameMode( MODE_UNKNOWN )
{
  connect( Hearthstone::Instance(), &Hearthstone::GameStarted, this, &ResultTracker::HandleHearthstoneStart );
  ResetResult();
}

ResultTracker::~ResultTracker() {
}

void ResultTracker::HandleHearthstoneStart() {
  DBG( "HandleHearthstoneStart" );

  mRegion = Hearthstone::Instance()->DetectRegion();
  DBG( "Region detected: %s", qt2cstr( mRegion ) );

  ResetResult();
  // Make sure we reset spectating mode when game is started
  mSpectating = false;
}

void ResultTracker::ResetResult() {
  mResult.Reset();
  mRanks.clear();
}

void ResultTracker::HandleOrder( GoingOrder order ) {
  DBG( "HandleOrder %s", ORDER_NAMES[ order ] );
  mResult.order = order;
}

void ResultTracker::HandleOutcome( Outcome outcome ) {
  DBG( "HandleOutcome %s", OUTCOME_NAMES[ outcome ] );
  mResult.outcome = outcome;
}

void ResultTracker::HandleOwnClass( HeroClass ownClass ) {
  DBG( "HandleOwnClass %s", CLASS_NAMES[ ownClass ] );
  mResult.hero = ownClass;
}

void ResultTracker::HandleOpponentClass( HeroClass opponentClass ) {
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

  if( !nowSpectating ) {
    ResetResult();
  }
}

void ResultTracker::HandleCardsPlayedUpdate( const ::CardHistoryList& cardsPlayed ) {
  DBG( "HandleCardsPlayedUpdate" );
  mResult.cardList = cardsPlayed;
}

void ResultTracker::HandleMatchEnd() {
  if( mSpectating ) {
    LOG( "Ignore spectated match" );
    ResetResult();
    return;
  }

  DBG( "HandleMatchEnd" );
  mResult.duration = mDurationTimer.elapsed() / 1000;
  mResult.added = QDateTime::currentDateTime();
  mResult.mode = mCurrentGameMode;
  mResult.region = mRegion;
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
