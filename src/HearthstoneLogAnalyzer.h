#pragma once

#include "hearthstone_log_watcher.h"

class HearthstoneLogAnalyzer : public QObject
{
  Q_OBJECT

public:
  HearthstoneLogAnalyzer();

signals:
  void PlayerDied(Player player); // Not triggered when conceding
  void CardPlayed(Player player, const string& cardId);
  void CardReturned(Player player, const string& cardId);
  void CoinReceived(Player player);

private slots:
  void HandleLogLine(const QString& line);

private:
  HearthstoneLogWatcher logWatcher;
};
