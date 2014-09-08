#include "Core.h"
#include "Tracker.h"

Core::Core()
  : mCurrentGameMode( MODE_UNKNOWN ),
    mGameRunning( false ),
    mOutcome( OUTCOME_UNKNOWN ),
    mOrder( ORDER_UNKNOWN ),
    mOwnClass( CLASS_UNKNOWN ),
    mOpponentClass( CLASS_UNKNOWN ),
    mCurrentResultTracked( false )
{
  mSceneManager.RegisterObserver( this );

  mTimer = new QTimer( this );
  connect( mTimer, SIGNAL( timeout() ), this, SLOT( Tick() ) );
  mTimer->start( 100 );

  // Connect log
  connect( &mLogTracker, SIGNAL( HandleOutcome(Outcome) ), this, SLOT( HandleOutcome(Outcome) ) );
  connect( &mLogTracker, SIGNAL( HandleOrder(GoingOrder) ), this, SLOT( HandleOrder(GoingOrder) ) );

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
  mOutcome       = OUTCOME_UNKNOWN;
  mOrder         = ORDER_UNKNOWN;
  mOwnClass      = CLASS_UNKNOWN;
  mOpponentClass = CLASS_UNKNOWN;
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
  mOrder = order;
}

void Core::HandleOutcome( Outcome outcome ) {
  // HandleOutcome can be triggered by log or by screen capture
  // So make sure we only upload the result once
  if( !mCurrentResultTracked ) {
    mCurrentResultTracked = true;

    mOutcome = outcome;
    TrackResult();
  }
}

void Core::HandleOwnClass( Class ownClass ) {
  mOwnClass = ownClass;
}

void Core::HandleOpponentClass( Class opponentClass ) {
  mOpponentClass = opponentClass;
}

void Core::TrackResult() {
  Tracker::Instance()->AddResult( mCurrentGameMode,
      mOutcome,
      mOrder,
      mOwnClass,
      mOpponentClass,
      mLogTracker.CardHistoryList() );

  // Reset
  mLogTracker.Reset();
  ResetResult();
}
