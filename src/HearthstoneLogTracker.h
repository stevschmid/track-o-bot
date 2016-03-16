#pragma once

#include "HearthstoneLogWatcher.h"
#include "Result.h"

#include <QMap>

class HearthstoneLogTracker : public QObject
{
  Q_OBJECT

private:
  QList< HearthstoneLogWatcher* > mLogWatchers;

  int mTurn;
  int mHeroPlayerId;
  bool mLegendTracked;
  bool mSpectating;
  bool mMatchConcluded;

  CardHistoryList mCardsPlayed;
  CardHistoryList mCardsDrawn;
  QMap< QString, int > mEntityIdByName;

  void CardPlayed( Player player, const QString& cardId, int internalId = 0 );
  void CardReturned( Player player, const QString& cardId, int internalId = 0 );
  void CardDrawn( Player player, const QString& cardId, int internalId = 0 );
  void CardUndrawn( Player player, const QString& cardId, int internalId = 0 );

  void ResolveCard( Player player, const QString& cardId, int internalId );

  int CurrentTurn() const;

  void Reset();

private slots:
  void HandleLogLine( const QString& line );

signals:
  void HandleMatchStart();
  void HandleMatchEnd();

  void HandleOutcome( Outcome outcome );
  void HandleOrder( GoingOrder order );
  void HandleGameMode( GameMode mode );
  void HandleOpponentClass( HeroClass opponentClass );
  void HandleOwnClass( HeroClass ownClass );
  void HandleLegend( int legend );
  void HandleTurn( int turnCounter );

  void HandleCardsPlayedUpdate( const ::CardHistoryList& cardsPlayed );
  void HandleCardsDrawnUpdate( const ::CardHistoryList& cardsDrawn );

  void HandleSpectating( bool nowSpectating );

public:
  HearthstoneLogTracker( QObject *parent = 0 );

  const CardHistoryList& CardHistoryList() const { return mCardsPlayed; }
};
