#pragma once

#include "HearthstoneLogTracker.h"
#include "Result.h"
#include "ResultQueue.h"
#include "RankClassifier.h"
#include "ReplayRecorder.h"

#include <QTimer>
#include <QTime>

#include <vector>

class ResultTracker : public QObject
{
  Q_OBJECT

private:
  QTime                 mDurationTimer;

  HearthstoneLogTracker mLogTracker;
  Result                mResult;

  std::vector<int>      mRanks;
  RankClassifier        mRankClassifier;

  ResultQueue           mResultsQueue;
  ReplayRecorder        mReplayRecorder;

  void ResetResult();
  void UploadResult();

  int DetermineRank();

private slots:
  void HandleMatchStart();
  void HandleMatchEnd( const ::CardHistoryList& cardHistoryList, bool wasSpectating );

  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( Class ownClass );
  void HandleOpponentClass( Class opponentClass );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );
  void HandleTurn( int turn, bool ownTurn );
  void HandleLegend( int legend );

public:
  ResultTracker();
  ~ResultTracker();
};
