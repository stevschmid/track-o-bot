#pragma once

#include "HearthstoneLogWatcher.h"
#include "HearthstoneLogLineHandler.h"
#include "Result.h"

class HearthstoneLogTracker : public QObject
{
  Q_OBJECT

private:
  QList< HearthstoneLogWatcher* > mLogWatchers;

  int mTurn;
  int mHeroPlayerId;
  bool mLegendTracked;
  bool mSpectating;
  QString mCurrentPlayerName;

  QList< int > mInitialDeckObjectIds;
  CardHistoryList mCardsPlayed;
  CardHistoryList mCardsDrawn;
  QMap< QString, int > mPlayerIdsByName;

  QList< HearthstoneLogLineHandler* > mLineHandlers;

  void RegisterHearthstoneLogLineHandler( const QString& module, const QString& call, const QString& regex, void (HearthstoneLogTracker::*)( const QVariantMap& args ) );

  void OnActionStart( const QVariantMap& args );
  void OnCreateGame( const QVariantMap& args );
  void OnLegendRank( const QVariantMap& args );
  void OnRanked( const QVariantMap& args );
  void OnSceneLoaded( const QVariantMap& args );
  void OnStartSpectating( const QVariantMap& args );
  void OnStopSpectating( const QVariantMap& args );
  void OnTagChange( const QVariantMap& args );
  void OnPlayerId( const QVariantMap& args );
  void OnPlayerName( const QVariantMap& args );
  void OnZoneChange( const QVariantMap& args );

  void CardPlayed( Player player, const QString& cardId, int internalId = 0 );
  void CardReturned( Player player, const QString& cardId, int internalId = 0 );
  void CardDrawn( Player player, const QString& cardId, int internalId = 0 );
  void CardUndrawn( Player player, const QString& cardId, int internalId = 0 );

  void ResolveCard( Player player, const QString& cardId, int internalId );

  int CurrentTurn() const;

  void Reset();

private slots:
  void HandleLogLine( const QString& module, const QString& line );

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
