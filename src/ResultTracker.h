#pragma once

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

  Result                mResult;
  GameMode              mCurrentGameMode;

  std::vector<int>      mRanks;
  RankClassifier        mRankClassifier;

  ResultQueue           mResultsQueue;

  QString               mRegion;

  void ResetResult();
  void UploadResult();

  int DetermineRank();

public slots:
  void HandleHearthstoneStart();
  void HandleMatchStart();
  void HandleMatchEnd();
  void HandleCardsPlayedUpdate( const ::CardHistoryList& cardsPlayed );
  void HandleSpectating( bool nowSpectating );

  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( HeroClass ownClass );
  void HandleOpponentClass( HeroClass opponentClass );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );
  void HandleTurn( int turn );
  void HandleLegend( int legend );

public:
  ResultTracker( QObject *parent = 0 );
  ~ResultTracker();
};
