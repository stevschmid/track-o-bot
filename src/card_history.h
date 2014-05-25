#pragma once

#include "log_watcher.h"

class CardHistoryItem {
public:
  bool myPlay;
  string cardId;

  CardHistoryItem(bool myPlay, const string& cardId)
    :myPlay(myPlay), cardId(cardId)
  {
  }
};
typedef vector<CardHistoryItem> CardHistoryList;

class CardHistory : public QObject
{
  Q_OBJECT

private:
  LogWatcher ingameLogWatcher;

  CardHistoryList list;

public:
  CardHistory();

  const CardHistoryList& List() const { return list; }
  void Clear();

private slots:
  void HandleLogLine(const QString& line);

};



