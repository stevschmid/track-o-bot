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

  CardHistoryList mCardHistoryList;

  void CardPlayed( Player player, const string& cardId );
  void CardReturned( Player player, const string& cardId );

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
  void HandleRankedLevel( int rank );

public:
  HearthstoneLogTracker();


  const CardHistoryList& CardHistoryList() const { return mCardHistoryList; }
};
