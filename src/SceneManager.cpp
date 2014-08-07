#include "SceneManager.h"
#include "Tracker.h"

SceneManager::SceneManager()
  : mCurrentScene( NULL )
{
  RegisterScene( new MainMenuScene );
  RegisterScene( new ConstructedScene );
  RegisterScene( new IngameScene );
  RegisterScene( new ArenaScene );
}

SceneManager::~SceneManager() {
  vector< Scene* >::iterator it;
  for( it = mScenes.begin(); it != mScenes.end(); ++it ) {
    delete *it;
  }
}

void SceneManager::RegisterScene( Scene *scene ) {
  mScenes.push_back( scene );
}

Scene* SceneManager::FindActiveScene() {
  vector<Scene*>::iterator it;
  for( it = mScenes.begin(); it != mScenes.end(); it++ ) {
    Scene *scene = *it;
    if( scene->Active() ) {
      return scene;
    }
  }
  return NULL;
}

void SceneManager::Update() {
  bool findNewScene = ( mCurrentScene == NULL );

  if( mCurrentScene ) {
    mCurrentScene->Update();
    findNewScene |= !mCurrentScene->Active();
  }

  // Keep the current scene if no other scene is detected yet.
  // Scene should never be NULL except at the beginning.
  if( findNewScene ) {
    Scene *newScene = FindActiveScene();
    if( newScene && newScene != mCurrentScene ) {
      // Notify our dear observers
      Notify( mCurrentScene, newScene );

      // Make sure we reset the previous' scene state (e.g. origin)
      if( mCurrentScene ) {
        mCurrentScene->Reset();
      }

      // Switch to the new scene
      mCurrentScene = newScene;
    }
  }
}

const Scene* SceneManager::ActiveScene() const {
  return mCurrentScene;
}

void SceneManager::RegisterObserver( SceneManagerObserver *observer ) {
  mObservers.push_back( observer );
}

void SceneManager::UnregisterObserver( SceneManagerObserver *observer ) {
  mObservers.erase( remove( mObservers.begin(), mObservers.end(), observer ), mObservers.end() );
}

void SceneManager::Notify( Scene *oldScene, Scene *newScene ) {
  vector< SceneManagerObserver* >::iterator it;
  for( it = mObservers.begin(); it != mObservers.end(); ++it ) {
    (*it)->SceneChanged( oldScene, newScene );
  }
}
