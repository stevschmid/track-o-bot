#pragma once

#include "HearthstoneLogWatcher.h"
#include "Result.h"

class HearthstoneLogTracker : public QObject
{
  Q_OBJECT

private:
  QList< HearthstoneLogWatcher* > mLogWatchers;

  int mTurnCounter;
  bool mHeroPowerUsed;
  int mHeroPlayerId;
  bool mLegendTracked;
  bool mSpectating;

  CardHistoryList mCardHistoryList;

  void CardPlayed( Player player, const string& cardId, int internalId = 0 );
  void CardReturned( Player player, const string& cardId );

  void SecretResolved( Player player, const string& cardId, int internalId );

  int CurrentTurn() const;

  void Reset();

private slots:
  void HandleLogLine( const QString& line );

signals:
  void HandleMatchStart();
  void HandleMatchEnd( const ::CardHistoryList& cardHistoryList );

  void HandleOutcome( Outcome outcome );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );
  void HandleOpponentClass( Class opponentClass );
  void HandleOwnClass( Class ownClass );
  void HandleLegend( int legend );
  void HandleTurn( int turnCounter );

  void HandleSpectating( bool nowSpectating );

public:
  HearthstoneLogTracker();

  const CardHistoryList& CardHistoryList() const { return mCardHistoryList; }
};
