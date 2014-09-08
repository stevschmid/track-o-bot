#pragma once

#include "SceneManager.h"
#include "Tracker.h"
#include "HearthstoneLogTracker.h"

#include <QTimer>
#include <QTime>

class Core : public QObject, public SceneManagerObserver
{
  Q_OBJECT

private:
  QTimer*               mTimer;
  SceneManager          mSceneManager;
  GameMode              mCurrentGameMode;
  bool                  mGameRunning;
  HearthstoneLogTracker mLogTracker;
  QTime                 mDurationTimer;

  Outcome               mOutcome;
  GoingOrder            mOrder;
  Class                 mOwnClass;
  Class                 mOpponentClass;

  bool                  mCurrentResultTracked;

  void ResetResult();
  void TrackResult();

private slots:
  void Tick();

  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( Class ownClass );
  void HandleOpponentClass( Class opponentClass );
  void HandleOrder( GoingOrder order );

public:
  Core();
  ~Core();

  void SceneChanged( Scene *oldScene, Scene *newScene );
};
