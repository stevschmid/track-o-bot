#pragma once

#include "HearthstoneLogWatcher.h"

class HearthstoneLogTracker : public QObject
{
  Q_OBJECT

private:
  HearthstoneLogWatcher mLogWatcher;

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
  void HandleMatchEnd( const ::CardHistoryList& cardHistoryList, bool wasSpectating );

  void HandleOutcome( Outcome outcome );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );
  void HandleOpponentClass( Class opponentClass );
  void HandleOwnClass( Class ownClass );
  void HandleRank( int rank );
  void HandleLegend( int legend );
  void HandleTurn( int turn );

public:
  HearthstoneLogTracker();

  const CardHistoryList& CardHistoryList() const { return mCardHistoryList; }
};
