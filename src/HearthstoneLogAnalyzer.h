#pragma once

#include "HearthstoneLogWatcher.h"

class HearthstoneLogAnalyzer : public QObject
{
  Q_OBJECT

private:
  HearthstoneLogWatcher mLogWatcher;

private slots:
  void HandleLogLine( const QString& line );

public:
  HearthstoneLogAnalyzer();

signals:
  void PlayerDied( Player player ); // Not triggered when conceding
  void CardPlayed( Player player, const string& cardId );
  void CardReturned( Player player, const string& cardId );
  void CoinReceived( Player player );
};
