#pragma once
#include "Json.h"

struct Result
{
public:

  Result(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList, int durationInSeconds)
        : mode(mode),
          outcome(outcome),
          order(order),
          ownClass(ownClass),
          opponentClass(opponentClass),
          cardHistoryList(cardHistoryList),
          durationInSeconds(durationInSeconds) {}
  
  QtJson::JsonObject AsJson() {
    QtJson::JsonObject result;
    
    result[ "coin" ]     = ( order == ORDER_SECOND );
    result[ "hero" ]     = CLASS_NAMES[ ownClass ];
    result[ "opponent" ] = CLASS_NAMES[ opponentClass ];
    result[ "win" ]      = ( outcome == OUTCOME_VICTORY );
    result[ "mode" ]     = MODE_NAMES[ mode ];
    result[ "duration" ] = durationInSeconds;
    
    QtJson::JsonArray card_history;
    for( CardHistoryList::const_iterator it = cardHistoryList.begin(); it != cardHistoryList.end(); ++it ) {
      QtJson::JsonObject item;
      item[ "turn" ] = (*it).turn;
      item[ "player" ] = (*it).player == PLAYER_SELF ? "me" : "opponent";
      item[ "card_id" ] = (*it).cardId.c_str();
      card_history.append(item);
    }
    result[ "card_history" ] = card_history;

    return result;
  }
  
  const GameMode mode;
  const Outcome outcome; GoingOrder order;
  const Class ownClass;
  const Class opponentClass;
  const CardHistoryList& cardHistoryList;
  const int durationInSeconds;
};