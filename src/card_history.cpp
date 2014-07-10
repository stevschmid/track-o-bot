#include "card_history.h"
#include "hearthstone.h"

CardHistory::CardHistory() {
  connect(&ingameLogWatcher, SIGNAL(LineAdded(QString)), this, SLOT(HandleLogLine(QString)));
}

void CardHistory::Clear() {
  list.clear();
}

void CardHistory::HandleLogLine(const QString& line) {
  if(!line.trimmed().isEmpty()) {
    QRegExp regex("ProcessChanges.*cardId=(\\w+).*zone from (.*) -> (.*)");
    if(regex.indexIn(line) != -1) {
      QStringList captures = regex.capturedTexts();
      QString cardId = captures[1];
      QString from = captures[2];
      QString to = captures[3];

      bool draw = from.contains("DECK") && to.contains("HAND");
      bool mulligan = from.contains("HAND") && to.contains("DECK");

      // Discarded cards by playing Soulfire, Doomguard etc.
      bool discard = from.contains("HAND") && to.contains("GRAVEYARD");

      if(!draw && !mulligan && !discard) {
        if(from.contains("FRIENDLY HAND")) {
          list.push_back(CardHistoryItem(PLAYER_SELF, cardId.toStdString()));
        } else if(from.contains("OPPOSING HAND")) {
          list.push_back(CardHistoryItem(PLAYER_OPPONENT, cardId.toStdString()));
        }
      }

      // Make sure we remove the "Choose One"-cards from the history
      // if we decide to withdraw them after a second of thought
      if(from.contains("FRIENDLY PLAY") && to.contains("FRIENDLY HAND")) {
        if(!list.empty() && list.back().cardId == cardId.toStdString()) {
          list.pop_back();
        }
      }

#ifdef _DEBUG
      LOG("Card %s from %s -> %s. (draw: %d, mulligan %d, discard %d)", cardId.toStdString().c_str(), from.toStdString().c_str(), to.toStdString().c_str(), draw, mulligan, discard);
#endif
    }
  }
}
