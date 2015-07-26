#pragma once

#include "Hearthstone.h"
#include "Tracker.h"
#include "HearthstoneLogTracker.h"

#include "RankClassifier.h"

#include <QTimer>
#include <QTime>

#include <vector>

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

  std::vector<int>      mRanks;
  int                   mLegend;

  bool                  mCurrentResultTracked;

  RankClassifier        mRankClassifier;

  void ResetResult();
  void UploadResult();

  int DetermineRank();

  bool mGameClientRestartRequired;
  void SetGameClientRestartRequired( bool restartRequired );

private slots:
  void HandleMatchStart();
  void HandleMatchEnd( const ::CardHistoryList& cardHistoryList, bool wasSpectating );

  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( Class ownClass );
  void HandleOpponentClass( Class opponentClass );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );
  void HandleTurn( int turn );
  void HandleLegend( int legend );

  void Tick();

signals:
  void HandleGameClientRestartRequired( bool restartRequired );

public:

  Core();
  ~Core();
};
