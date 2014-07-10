#pragma once

#include "hearthstone_log_watcher.h"

class CardHistoryItem {
public:
  Player player;
  string cardId;

  CardHistoryItem(Player player, const string& cardId)
    :player(player), cardId(cardId)
  {
  }
};
typedef vector<CardHistoryItem> CardHistoryList;

class CardHistory : public QObject
{
  Q_OBJECT

private:
  HearthstoneLogWatcher ingameLogWatcher;

  CardHistoryList list;

public:
  CardHistory();

  const CardHistoryList& List() const { return list; }
  void Clear();

private slots:
  void HandleLogLine(const QString& line);

};

