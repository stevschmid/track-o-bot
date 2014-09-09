#pragma once

#include "Hearthstone.h"
#include "Tracker.h"
#include "HearthstoneLogTracker.h"

#include <QTimer>
#include <QTime>

class Core : public QObject
{
  Q_OBJECT

private:
  QTimer*               mTimer;
  bool                  mGameRunning;
  HearthstoneLogTracker mLogTracker;
  QTime                 mDurationTimer;

  GameMode              mGameMode;
  Outcome               mOutcome;
  GoingOrder            mOrder;
  Class                 mOwnClass;
  Class                 mOpponentClass;
  int                   mDuration;

  bool                  mCurrentResultTracked;

  void ResetResult();
  void UploadResult();


private slots:
  void HandleMatchStart();
  void HandleMatchEnd();

  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( Class ownClass );
  void HandleOpponentClass( Class opponentClass );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );

  void Tick();

public:
  Core();
  ~Core();
};
