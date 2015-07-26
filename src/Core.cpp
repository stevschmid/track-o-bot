#include "Core.h"
#include "Tracker.h"

#include <map>

Core::Core()
  : mGameRunning( false ),
    mGameMode( MODE_UNKNOWN ),
    mOutcome( OUTCOME_UNKNOWN ),
    mOrder( ORDER_UNKNOWN ),
    mOwnClass( CLASS_UNKNOWN ),
    mOpponentClass( CLASS_UNKNOWN ),
    mDuration( 0 ),
    mLegend( LEGEND_UNKNOWN ),
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
  connect( &mLogTracker, SIGNAL( HandleTurn(int) ), this, SLOT( HandleTurn(int) ) );

  connect( &mLogTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( &mLogTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&, bool) ), this, SLOT( HandleMatchEnd(const ::CardHistoryList&, bool) ) );

  ResetResult();
}

Core::~Core() {
  delete mTimer;
}

void Core::ResetResult() {
  mGameMode      = MODE_UNKNOWN;
  mOutcome       = OUTCOME_UNKNOWN;
  mOrder         = ORDER_UNKNOWN;
  mOwnClass      = CLASS_UNKNOWN;
  mOpponentClass = CLASS_UNKNOWN;
  mDuration      = 0;
  mCardHistoryList.clear();
  mLegend        = LEGEND_UNKNOWN;

  mRanks.clear();
}

void Core::Tick() {
  bool wasGameRunning = mGameRunning;
  mGameRunning = Hearthstone::Instance()->Running();

  if( wasGameRunning != mGameRunning ) {
    if( mGameRunning ) {
      LOG("Hearthstone found");
    } else {
      LOG("Hearthstone was closed");
      Hearthstone::Instance()->SetRestartRequired( false );
    }
  }

  if( mGameClientRestartRequired != Hearthstone::Instance()->RestartRequired() ) {
    mGameClientRestartRequired = Hearthstone::Instance()->RestartRequired();
    emit HandleGameClientRestartRequired( mGameClientRestartRequired );
  }
}

void Core::HandleOrder( GoingOrder order ) {
  DEBUG( "HandleOrder %s", ORDER_NAMES[ order ] );
  mOrder = order;
}

void Core::HandleOutcome( Outcome outcome ) {
  DEBUG( "HandleOutcome %s", OUTCOME_NAMES[ outcome ] );
  mOutcome = outcome;
}

void Core::HandleOwnClass( Class ownClass ) {
  DEBUG( "HandleOwnClass %s", CLASS_NAMES[ ownClass ] );
  mOwnClass = ownClass;
}

void Core::HandleOpponentClass( Class opponentClass ) {
  DEBUG( "HandleOpponentClass %s", CLASS_NAMES[ opponentClass ] );
  mOpponentClass = opponentClass;
}

void Core::HandleMatchStart() {
  DEBUG( "HandleMatchStart" );
  mDurationTimer.start();
}

void Core::HandleMatchEnd( const ::CardHistoryList& cardHistoryList, bool wasSpectating ) {
  if( wasSpectating ) {
    LOG( "Ignore spectated match" );
    ResetResult();
    return;
  }

  DEBUG( "HandleMatchEnd" );
  mCardHistoryList = cardHistoryList;
  mDuration = mDurationTimer.elapsed() / 1000;
  UploadResult();
}

void Core::HandleGameMode( GameMode mode ) {
  DEBUG( "HandleGameMode %s", MODE_NAMES[ mode ] );
  mGameMode = mode;
}

void Core::HandleLegend( int legend ) {
  DEBUG( "Set Legend %d", legend );
  mLegend = legend;
}

void Core::HandleTurn( int turn ) {
  int rank = mRankClassifier.DetectCurrentRank();
  mRanks.push_back( rank );
  DEBUG( "Turn %d. Set Rank %d", turn, rank );
}

// Screen capture can be tricky
// So capture the rank a few times during the game
// and the majority vote will be the determined rank
int Core::DetermineRank() {
  std::map< int, int > votesByRank;

  int maxVote = 0;
  int maxRank = RANK_UNKNOWN;

  for( std::vector<int>::iterator it = mRanks.begin(); it != mRanks.end(); ++it ) {
    votesByRank[ *it ] += 1;
  }

  for( std::map<int,int>::iterator it = votesByRank.begin(); it != votesByRank.end(); ++it ) {
    DEBUG( "Rank %d has %d votes", (*it).first, (*it).second );
    if( (*it).second > maxVote ) {
      maxVote = (*it).second;
      maxRank = (*it).first;
    }
  }

  return maxRank;
}

void Core::UploadResult() {
  DEBUG( "UploadResult" );

  int rank = DetermineRank();
  DEBUG( "Determined Rank: %d", rank );

  Tracker::Instance()->AddResult( mGameMode,
      mOutcome,
      mOrder,
      mOwnClass,
      mOpponentClass,
      mLogTracker.CardHistoryList(),
      mDuration,
      rank,
      mLegend );

  ResetResult();
}
