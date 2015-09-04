#include "Core.h"

#include "Hearthstone.h"

Core::Core()
  : mGameRunning( false ),
    mGameClientRestartRequired( false )
{
  mTimer = new QTimer( this );
  connect( mTimer, SIGNAL( timeout() ), this, SLOT( Tick() ) );
  mTimer->start( 1000 );
}

Core::~Core() {
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
