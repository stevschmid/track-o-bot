#pragma once

#include "Hearthstone.h"
#include "Tracker.h"
#include "HearthstoneLogTracker.h"
#include "Result.h"
#include "ResultQueue.h"
#include "BattleBoard.h"

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
  bool                  mSpectating;
  HearthstoneLogTracker mLogTracker;
  QTime                 mDurationTimer;


  Result                mResult;
  std::vector<int>      mRanks;

  RankClassifier        mRankClassifier;
  ResultQueue           mResultsQueue;
  Battleboard           mBattleboard;

  void ResetResult();
  void UploadResult();

  int DetermineRank();

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
  void HandleTurn( int turnCounter );
  void HandleLegend( int legend );
  void HandleSpectating( bool nowSpectating );

  void Tick();

signals:
  void HandleGameClientRestartRequired( bool restartRequired );

public:

  Core();
  ~Core();
};
