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

  if( newScene->Name() ==  "Ingame" ) {
    if( oldScene ) {
      if( oldScene->Name() == "Constructed" ) {
        ConstructedScene *constructed = ( ConstructedScene* )oldScene;
        mCurrentGameMode = constructed->GameMode();
      }
      if( oldScene->Name() == "Arena" ) {
        mCurrentGameMode = MODE_ARENA;
      }
    }
  }

  if( oldScene && oldScene->Name() == "Ingame" ) {
    IngameScene *ingame = ( IngameScene* )oldScene;

    Tracker::Instance()->AddResult(
        mCurrentGameMode,
        ingame->Outcome(),
        ingame->GoingOrder(),
        ingame->OwnClass(),
        ingame->OpponentClass(),
        ingame->CardHistoryList() );
  }
}
