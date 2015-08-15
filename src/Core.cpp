#include "Core.h"

#include <map>

Core::Core()
  :
    mGameRunning( false ),
    mSpectating( false ),
    mGameClientRestartRequired( false )
{
  mTimer = new QTimer( this );
  connect( mTimer, SIGNAL( timeout() ), this, SLOT( Tick() ) );
  mTimer->start( 1000 );

  // Connect log
  connect( &mLogTracker, SIGNAL( HandleOutcome(Outcome) ), this, SLOT( HandleOutcome(Outcome) ) );
  connect( &mLogTracker, SIGNAL( HandleOrder(GoingOrder) ), this, SLOT( HandleOrder(GoingOrder) ) );
  connect( &mLogTracker, SIGNAL( HandleOwnClass(Class) ), this, SLOT( HandleOwnClass(Class) ) ) ;
  connect( &mLogTracker, SIGNAL( HandleOpponentClass(Class) ), this, SLOT( HandleOpponentClass(Class) ) );
  connect( &mLogTracker, SIGNAL( HandleGameMode(GameMode) ), this, SLOT( HandleGameMode(GameMode) ) );
  connect( &mLogTracker, SIGNAL( HandleLegend(int) ), this, SLOT( HandleLegend(int) ) );
  connect( &mLogTracker, SIGNAL( HandleTurn(int, bool) ), this, SLOT( HandleTurn(int, bool) ) );

  connect( &mLogTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( &mLogTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&) ), this, SLOT( HandleMatchEnd(const ::CardHistoryList&) ) );
  connect( &mLogTracker, SIGNAL( HandleSpectating(bool) ), this, SLOT( HandleSpectating(bool) ) );

  ResetResult();
}

Core::~Core() {
}

void Core::ResetResult() {
  mResult.Reset();
  mRanks.clear();
  mSpectating = false;
}

void Core::Tick() {
  bool wasGameRunning = mGameRunning;
  mGameRunning = Hearthstone::Instance()->Running();

  if( wasGameRunning != mGameRunning ) {
    if( mGameRunning ) {
      LOG("Hearthstone window found");
    } else {
      LOG("Hearthstone window was closed");
      Hearthstone::Instance()->SetRestartRequired( false );
    }
  }

  if( mGameClientRestartRequired != Hearthstone::Instance()->RestartRequired() ) {
    mGameClientRestartRequired = Hearthstone::Instance()->RestartRequired();
    emit HandleGameClientRestartRequired( mGameClientRestartRequired );
  }
}

void Core::HandleOrder( GoingOrder order ) {
  DBG( "HandleOrder %s", ORDER_NAMES[ order ] );
  mResult.order = order;
}

void Core::HandleOutcome( Outcome outcome ) {
  DBG( "HandleOutcome %s", OUTCOME_NAMES[ outcome ] );
  mResult.outcome = outcome;
}

void Core::HandleOwnClass( Class ownClass ) {
  DBG( "HandleOwnClass %s", CLASS_NAMES[ ownClass ] );
  mResult.hero = ownClass;
}

void Core::HandleOpponentClass( Class opponentClass ) {
  DBG( "HandleOpponentClass %s", CLASS_NAMES[ opponentClass ] );
  mResult.opponent = opponentClass;
}

void Core::HandleMatchStart() {
  DBG( "HandleMatchStart" );
  mDurationTimer.start();
}

void Core::HandleMatchEnd( const ::CardHistoryList& cardHistoryList ) {
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

void Core::HandleGameMode( GameMode mode ) {
  DBG( "HandleGameMode %s", MODE_NAMES[ mode ] );
  mResult.mode = mode;
}

void Core::HandleLegend( int legend ) {
  DBG( "Set Legend %d", legend );
  mResult.legend = legend;
}

void Core::HandleTurn( int turn, bool ownTurn ) {
  int rank = mRankClassifier.DetectCurrentRank();
  mRanks.push_back( rank );
  DBG( "Turn %d (my turn %d). Set Rank %d", turn, ownTurn, rank );
}

void Core::HandleSpectating( bool nowSpectating ) {
  DBG( "HandleSpectacting %d", nowSpectating );
  mSpectating = nowSpectating;
}

// Screen capture can be tricky
// So capture the rank a few times during the game
// and the majority vote will be the determined rank
int Core::DetermineRank() {
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

void Core::UploadResult() {
  DBG( "UploadResult" );

  mResult.rank = DetermineRank();
  DBG( "Determined Rank: %d", mResult.rank );

  mResultsQueue.Add( mResult );
  ResetResult();
}
