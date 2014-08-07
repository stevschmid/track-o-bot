#pragma once

#include "HearthstoneLogWatcher.h"

class HearthstoneLogTracker : public QObject
{
  Q_OBJECT

private:
  HearthstoneLogWatcher mLogWatcher;

  Outcome mOutcome;
  GoingOrder mOrder;
  CardHistoryList mCardHistoryList;

  void PlayerDied( Player player ); // Not triggered when conceding
  void CardPlayed( Player player, const string& cardId );
  void CardReturned( Player player, const string& cardId );
  void CoinReceived( Player player );

  void Reset();

private slots:
  void HandleLogLine( const QString& line );

public:
  HearthstoneLogTracker();

  Outcome Outcome() const { return mOutcome; }
  GoingOrder Order() const { return mOrder; }
  const CardHistoryList& CardHistoryList() const { return mCardHistoryList; }

};
