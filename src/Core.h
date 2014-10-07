#pragma once

#include "Hearthstone.h"
#include "Tracker.h"
#include "HearthstoneLogTracker.h"
#include "IResultSink.h"
#include "Result.h"

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
  CardHistoryList       mCardHistoryList;

  bool                  mCurrentResultTracked;
  
  QHash<QString, IResultSink*> mResultSinks;
  
  void ResetResult();
  void ArchiveResult();

  bool mGameClientRestartRequired;
  void SetGameClientRestartRequired( bool restartRequired );

private slots:
  void HandleMatchStart();
  void HandleMatchEnd( const ::CardHistoryList& cardHistoryList );

  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( Class ownClass );
  void HandleOpponentClass( Class opponentClass );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );

  void Tick();

signals:
  void HandleGameClientRestartRequired( bool restartRequired );

public:

  Core();
  ~Core();
};
