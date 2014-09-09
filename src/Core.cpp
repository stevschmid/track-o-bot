#include "Core.h"
#include "Tracker.h"

Core::Core()
  : mCurrentGameMode( MODE_UNKNOWN ),
    mGameRunning( false ),
    mGameMode( MODE_UNKNOWN ),
    mOutcome( OUTCOME_UNKNOWN ),
    mOrder( ORDER_UNKNOWN ),
    mOwnClass( CLASS_UNKNOWN ),
    mOpponentClass( CLASS_UNKNOWN ),
    mDuration( 0 ),
    mCurrentResultTracked( false )
{
  mSceneManager.RegisterObserver( this );

  mTimer = new QTimer( this );
  connect( mTimer, SIGNAL( timeout() ), this, SLOT( Tick() ) );
  mTimer->start( 100 );

  // Connect log
  connect( &mLogTracker, SIGNAL( HandleOutcome(Outcome) ), this, SLOT( HandleOutcome(Outcome) ) );
  connect( &mLogTracker, SIGNAL( HandleOrder(GoingOrder) ), this, SLOT( HandleOrder(GoingOrder) ) );
  connect( &mLogTracker, SIGNAL( HandleOwnClass(Class) ), this, SLOT( HandleOwnClass(Class) ) ) ;
  connect( &mLogTracker, SIGNAL( HandleOpponentClass(Class) ), this, SLOT( HandleOpponentClass(Class) ) );
  connect( &mLogTracker, SIGNAL( HandleGameMode(GameMode) ), this, SLOT( HandleGameMode(GameMode) ) );

  // Connect scene (screen capture)
  IngameScene *ingameScene = ( IngameScene* )mSceneManager.FindScene( "Ingame" );
  connect( ingameScene, SIGNAL( HandleOutcome(Outcome) ), this, SLOT( HandleOutcome(Outcome) ) );
  connect( ingameScene, SIGNAL( HandleOrder(GoingOrder) ), this, SLOT( HandleOrder(GoingOrder) ) );
  connect( ingameScene, SIGNAL( HandleOwnClass(Class) ), this, SLOT( HandleOwnClass(Class) ) ) ;
  connect( ingameScene, SIGNAL( HandleOpponentClass(Class) ), this, SLOT( HandleOpponentClass(Class) ) );

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
}

void Core::Tick() {
  bool wasGameRunning = mGameRunning;
  mGameRunning = Hearthstone::Instance()->Running();

  if( wasGameRunning != mGameRunning ) {
    if( mGameRunning ) {
      LOG("Hearthstone found");
    } else {
      LOG("Hearthstone was closed");
    }
  }

  if( mGameRunning ) {
    mSceneManager.Update();
  }
}

void Core::SceneChanged( Scene *oldScene, Scene *newScene ) {
  LOG( "Scene %s", newScene->Name().c_str() );

  if( newScene->Name() == "ClassSelection" ) {
    // The same class selection screen is used in many different places (practice, naxx, friendly)
    // Assume friendly game mode for all other scene transitions (except ingame)
    if( oldScene ) {
      if( oldScene->Name() == "SoloAdventures" ) {
        mCurrentGameMode = MODE_SOLO_ADVENTURES;
      } else if( oldScene->Name() != "Ingame" ) {
        mCurrentGameMode = MODE_FRIENDLY;
      }
    }
  }

  if( newScene->Name() == "Ingame" ) {
    mCurrentResultTracked = false;

    if( oldScene ) {
      if( oldScene->Name() == "Constructed" ) {
        ConstructedScene *constructed = ( ConstructedScene* )oldScene;
        mCurrentGameMode = constructed->GameMode();
      } else if( oldScene->Name() == "Arena" ) {
        mCurrentGameMode = MODE_ARENA;
      }
    }
  }
}

void Core::HandleOrder( GoingOrder order ) {
  DEBUG( "HandleOrder %s", ORDER_NAMES[ order ] );
  mOrder = order;
}

void Core::HandleOutcome( Outcome outcome ) {
  DEBUG( "HandleOutcome %s", OUTCOME_NAMES[ outcome ] );

  // HandleOutcome can be triggered by log or by screen capture
  // So make sure we only upload the result once
  if( !mCurrentResultTracked ) {
    mCurrentResultTracked = true;

    mOutcome = outcome;
    TrackResult();
  }
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

void Core::HandleMatchEnd() {
  DEBUG( "HandleMatchEnd" );
  mDuration = mDurationTimer.elapsed() / 1000;
}

void Core::HandleGameMode( GameMode mode ) {
  DEBUG( "HandleGameMode %s", MODE_NAMES[ mode ] );
  mGameMode = mode;
}

void Core::TrackResult() {
  DEBUG( "TrackResult" );

  int durationInSeconds = mDurationTimer.elapsed() / 1000;
  Tracker::Instance()->AddResult( mCurrentGameMode,
      mOutcome,
      mOrder,
      mOwnClass,
      mOpponentClass,
      mLogTracker.CardHistoryList(),
      durationInSeconds );

  // Reset
  mLogTracker.Reset();
  ResetResult();
}

