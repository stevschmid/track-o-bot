#include "Core.h"
#include "Tracker.h"

Core::Core()
  : mGameRunning( false ),
    mGameMode( MODE_UNKNOWN ),
    mOutcome( OUTCOME_UNKNOWN ),
    mOrder( ORDER_UNKNOWN ),
    mOwnClass( CLASS_UNKNOWN ),
    mOpponentClass( CLASS_UNKNOWN ),
    mDuration( 0 ),
    mRank( RANK_UNKNOWN ),
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
  connect( &mLogTracker, SIGNAL( HandleRank(int) ), this, SLOT( HandleRank(int) ) );
  connect( &mLogTracker, SIGNAL( HandleLegend(int) ), this, SLOT( HandleLegend(int) ) );

  connect( &mLogTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( &mLogTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&) ), this, SLOT( HandleMatchEnd(const ::CardHistoryList&) ) );

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
  mRank          = RANK_UNKNOWN;
  mLegend        = LEGEND_UNKNOWN;
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

void Core::HandleMatchEnd( const ::CardHistoryList& cardHistoryList ) {
  DEBUG( "HandleMatchEnd" );
  mCardHistoryList = cardHistoryList;
  mDuration = mDurationTimer.elapsed() / 1000;
  UploadResult();
}

void Core::HandleGameMode( GameMode mode ) {
  DEBUG( "HandleGameMode %s", MODE_NAMES[ mode ] );
  mGameMode = mode;
}

void Core::HandleRank( int rank ) {
  DEBUG( "Set Rank %d", rank );
  mRank = rank;
}

void Core::HandleLegend( int legend ) {
  DEBUG( "Set Legend %d", legend );
  mLegend = legend;
}

void Core::UploadResult() {
  DEBUG( "UploadResult" );

  Tracker::Instance()->AddResult( mGameMode,
      mOutcome,
      mOrder,
      mOwnClass,
      mOpponentClass,
      mLogTracker.CardHistoryList(),
      mDuration,
      mRank,
      mLegend );

  ResetResult();
}
