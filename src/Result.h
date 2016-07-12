#pragma once

#include "Local.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QString>
#include <QList>

typedef enum {
  PLAYER_SELF = 0,
  PLAYER_OPPONENT,
  PLAYER_UNKNOWN
} Player;

const char PLAYER_NAMES[][128] = {
  "self",
  "opponent",
  "unknown"
};

typedef enum {
  ORDER_FIRST = 0,
  ORDER_SECOND,
  ORDER_UNKNOWN
} GoingOrder;

const char ORDER_NAMES[][128] = {
  "first",
  "second",
  "unknown"
};

typedef enum {
  MODE_RANKED = 0,
  MODE_CASUAL,
  MODE_SOLO_ADVENTURES,
  MODE_ARENA,
  MODE_FRIENDLY,
  MODE_TAVERN_BRAWL,
  MODE_UNKNOWN
} GameMode;

const char MODE_NAMES[][128] = {
  "ranked",
  "casual",
  "solo",
  "arena",
  "friendly",
  "unknown"
};

typedef enum {
  OUTCOME_VICTORY = 0,
  OUTCOME_DEFEAT,
  OUTCOME_UNKNOWN
} Outcome;

const char OUTCOME_NAMES[][128] = {
  "victory",
  "defeat",
  "unknown"
};

typedef enum {
  CLASS_PRIEST = 0,
  CLASS_ROGUE,
  CLASS_MAGE,
  CLASS_PALADIN,
  CLASS_WARRIOR,
  CLASS_WARLOCK,
  CLASS_HUNTER,
  CLASS_SHAMAN,
  CLASS_DRUID,
  CLASS_UNKNOWN
} HeroClass;

const char CLASS_NAMES[][128] = {
  "priest",
  "rogue",
  "mage",
  "paladin",
  "warrior",
  "warlock",
  "hunter",
  "shaman",
  "druid",
  "unknown"
};
#define NUM_CLASSES 9

#define RANK_UNKNOWN   0
#define LEGEND_UNKNOWN  0

class CardHistoryItem {
public:
  int turn;
  Player player;
  QString cardId;
  int internalId;

  CardHistoryItem( int turn, Player player, const QString& cardId, int internalId = 0 )
    : turn( turn ), player( player ), cardId( cardId ), internalId( internalId )
  {
  }
};
typedef QList< CardHistoryItem > CardHistoryList;

class Result {
public:
  GameMode mode;
  Outcome outcome;
  GoingOrder order;

  HeroClass hero;
  HeroClass opponent;

  CardHistoryList cardList;

  int rank;
  int legend;

  int duration;
  QDateTime added;

  QString region;

  Result() {
    Reset();
  }

  void Reset() {
    mode = MODE_UNKNOWN;
    outcome = OUTCOME_UNKNOWN;
    order = ORDER_UNKNOWN;

    hero = CLASS_UNKNOWN;
    opponent = CLASS_UNKNOWN;

    cardList.clear();

    rank = RANK_UNKNOWN;
    legend = LEGEND_UNKNOWN;

    duration = 0;

    region = "";
  }

  QJsonObject AsJson() const {
    QJsonObject result;
    result[ "coin" ]     = ( order == ORDER_SECOND );
    result[ "hero" ]     = CLASS_NAMES[ hero ];
    result[ "opponent" ] = CLASS_NAMES[ opponent ];
    result[ "win" ]      = ( outcome == OUTCOME_VICTORY );
    result[ "mode" ]     = MODE_NAMES[ mode ];
    result[ "duration" ] = duration;
    result[ "added" ]    = added.toTimeSpec( Qt::OffsetFromUTC ).toString( Qt::ISODate );
    if( !region.isEmpty() ) {
      result[ "region" ]    = region;
    }

    if( mode == MODE_RANKED && rank != RANK_UNKNOWN && legend == LEGEND_UNKNOWN ) {
      result[ "rank" ] = rank;
    }
    if( mode == MODE_RANKED && legend != LEGEND_UNKNOWN ) {
      result[ "legend" ] = legend;
    }

    QJsonArray card_history;
    for( const CardHistoryItem& chi : cardList ) {
      QJsonObject item;

      if( chi.cardId.isEmpty() )
        continue;

      item[ "turn" ] = chi.turn;
      item[ "player" ] = chi.player == PLAYER_SELF ? "me" : "opponent";
      item[ "card_id" ] = chi.cardId;
      card_history.append(item);
    }
    result[ "card_history" ] = card_history;
    return result;
  }
};


