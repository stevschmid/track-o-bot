#include "Core.h"
#include "Tracker.h"

Core::Core()
  : mCurrentGameMode( MODE_UNKNOWN ), mGameRunning( false )
{
  mSceneManager.RegisterObserver( this );

  mTimer = new QTimer( this );
  connect( mTimer, SIGNAL( timeout() ), this, SLOT( Tick() ) );
  mTimer->start( 100 );
}

Core::~Core() {
  delete mTimer;
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
    // Assume friendly game mode for all other scene transitions
    if( oldScene->Name() == "SoloAdventures" ) {
      mCurrentGameMode = MODE_SOLO_ADVENTURES;
    } else {
      mCurrentGameMode = MODE_FRIENDLY;
    }
  }

  if( newScene->Name() == "Ingame" ) {
    if( oldScene ) {
      if( oldScene->Name() == "Constructed" ) {
        ConstructedScene *constructed = ( ConstructedScene* )oldScene;
        mCurrentGameMode = constructed->GameMode();
      } else if( oldScene->Name() == "Arena" ) {
        mCurrentGameMode = MODE_ARENA;
      }
    }
  }

  if( oldScene && oldScene->Name() == "Ingame" ) {
    IngameScene *ingame = ( IngameScene* )oldScene;

    GameMode        gameMode        = mCurrentGameMode;
    Outcome         outcome         = ingame->Outcome();
    GoingOrder      order           = ingame->GoingOrder();
    Class           ownClass        = ingame->OwnClass();
    Class           opponentClass   = ingame->OpponentClass();
    CardHistoryList cardHistoryList = mLogTracker.CardHistoryList();

    // Use the log as fallback
    if( outcome == OUTCOME_UNKNOWN ) {
      outcome = mLogTracker.Outcome();
    }

    if( order == ORDER_UNKNOWN ) {
      order = mLogTracker.Order();
    }

    Tracker::Instance()->AddResult( gameMode, outcome, order, ownClass, opponentClass, cardHistoryList );
  }
}
