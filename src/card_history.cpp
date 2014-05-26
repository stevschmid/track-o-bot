#include "card_history.h"
#include "hearthstone.h"

CardHistory::CardHistory()
  :ingameLogWatcher(Hearthstone::Instance()->LogPath().c_str())
{
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

      if(!to.contains("DECK") && !from.contains("DECK")) { // ignore mulligan stuff
        if(from.contains("FRIENDLY HAND")) {
          list.push_back(CardHistoryItem(true, cardId.toStdString()));
        } else if(from.contains("OPPOSING HAND")) {
          list.push_back(CardHistoryItem(false, cardId.toStdString()));
        }
      }

#ifdef _DEBUG
      LOG("Card %s from %s -> %s", cardId.toStdString().c_str(), from.toStdString().c_str(), to.toStdString().c_str());
#endif
    }
  }
}
