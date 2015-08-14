#pragma once

#include "Local.h"

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
} Class;

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
  string cardId;
  int internalId;

  CardHistoryItem( int turn, Player player, const string& cardId, int internalId = 0 )
    : turn( turn ), player( player ), cardId( cardId ), internalId( internalId )
  {
  }
};
typedef vector< CardHistoryItem > CardHistoryList;

class Result {
public:
  GameMode mode;
  Outcome outcome;
  GoingOrder order;

  Class hero;
  Class opponent;

  CardHistoryList cardList;

  int rank;
  int legend;

  int duration;

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
  }
};

