#pragma once

#include "HearthstoneLogTracker.h"
#include "Result.h"
#include "ResultQueue.h"
#include "RankClassifier.h"

#include <QTimer>
#include <QTime>

#include <vector>

class ResultTracker : public QObject
{
  Q_OBJECT

private:
  QTime                 mDurationTimer;
  bool                  mSpectating;

  HearthstoneLogTracker mLogTracker;
  Result                mResult;
  GameMode              mCurrentGameMode;

  std::vector<int>      mRanks;
  RankClassifier        mRankClassifier;

  ResultQueue           mResultsQueue;

  void ResetResult();
  void UploadResult();

  int DetermineRank();

private slots:
  void HandleMatchStart();
  void HandleMatchEnd( const ::CardHistoryList& cardHistoryList );
  void HandleSpectating( bool nowSpectating );

  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( Class ownClass );
  void HandleOpponentClass( Class opponentClass );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );
  void HandleTurn( int turn );
  void HandleLegend( int legend );

public:
  ResultTracker();
  ~ResultTracker();
};
