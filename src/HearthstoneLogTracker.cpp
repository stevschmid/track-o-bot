#include "HearthstoneLogTracker.h"

#include <QRegExp>
#include <QStringList>

// Hero Power Card Ids: Auto generated
const int NUM_HERO_POWER_CARDS = 43;
const char HERO_POWER_CARD_IDS[NUM_HERO_POWER_CARDS][32] = {
"CS1h_001" /* Lesser Heal */, "CS2_017" /* Shapeshift */, "CS2_034" /* Fireblast */, "CS2_049" /* Totemic Call */, "CS2_056" /* Life Tap */, "CS2_083b" /* Dagger Mastery */, "CS2_101" /* Reinforce */, "CS2_102" /* Armor Up! */, "DS1h_292" /* Steady Shot */, "EX1_625t" /* Mind Spike */, "EX1_625t2" /* Mind Shatter */, "EX1_tk33" /* INFERNO! */, "NAX10_03" /* Hateful Strike */, "NAX10_03H" /* Hateful Strike */, "NAX11_02" /* Poison Cloud */, "NAX11_02H" /* Poison Cloud */, "NAX12_02" /* Decimate */, "NAX12_02H" /* Decimate */, "NAX13_02" /* Polarity Shift */, "NAX14_02" /* Frost Breath */, "NAX15_02" /* Frost Blast */, "NAX15_02H" /* Frost Blast */, "NAX15_04" /* Chains */, "NAX15_04H" /* Chains */, "NAX1_04" /* Skitter */, "NAX1h_04" /* Skitter */, "NAX2_03" /* Rain of Fire */, "NAX2_03H" /* Rain of Fire */, "NAX3_02" /* Web Wrap */, "NAX3_02H" /* Web Wrap */, "NAX4_04" /* Raise Dead */, "NAX4_04H" /* Raise Dead */, "NAX5_02" /* Eruption */, "NAX5_02H" /* Eruption */, "NAX6_02" /* Necrotic Aura */, "NAX6_02H" /* Necrotic Aura */, "NAX7_03" /* Unbalancing Strike */, "NAX7_03H" /* Unbalancing Strike */, "NAX8_02" /* Harvest */, "NAX8_02H" /* Harvest */, "NAX9_06" /* Unholy Shadow */, "TU4d_003" /* Shotgun Blast */, "TU4e_002" /* Flames of Azzinoth */
};

// Hero Ids: Must match Class enum
const int NUM_HEROES = 9;
const char HERO_IDS[NUM_HEROES][32] = {
  "HERO_09", // CLASS_PRIEST = 0,
  "HERO_03", // CLASS_ROGUE,
  "HERO_08", // CLASS_MAGE,
  "HERO_04", // CLASS_PALADIN,
  "HERO_01", // CLASS_WARRIOR,
  "HERO_07", // CLASS_WARLOCK,
  "HERO_05", // CLASS_HUNTER,
  "HERO_02", // CLASS_SHAMAN,
  "HERO_06" // CLASS_DRUID,
};

Q_DECLARE_METATYPE( ::CardHistoryList );

HearthstoneLogTracker::HearthstoneLogTracker()
  : mTurnCounter( 0 ), mHeroPowerUsed( false ), mHeroPlayerId( 0 )
{
  qRegisterMetaType< ::CardHistoryList >( "CardHistoryList" );

  connect( &mLogWatcher, SIGNAL( LineAdded(QString) ), this, SLOT( HandleLogLine(QString) ) );
  Reset();
}

void HearthstoneLogTracker::Reset() {
  mTurnCounter = 0;
  mHeroPowerUsed = false;
  mCardHistoryList.clear();
}

void HearthstoneLogTracker::HandleLogLine( const QString& line ) {
  if( line.trimmed().isEmpty() )
    return;

  // CardPlayed / CardReturned / PlayerDied
  QRegExp regex( "ProcessChanges.*cardId=(\\w+).*zone from (.*) -> (.*)" );
  if( regex.indexIn(line) != -1 ) {
    QStringList captures = regex.capturedTexts();
    QString cardId = captures[1];
    QString from = captures[2];
    QString to = captures[3];

    bool draw = from.contains( "DECK" ) && to.contains( "HAND" );
    bool mulligan = from.contains( "HAND" ) && to.contains( "DECK" );

    // Discarded cards by playing Soulfire, Doomguard etc.
    bool discard = from.contains( "HAND" ) && to.contains( "GRAVEYARD" );

    if( !draw && !mulligan && !discard ) {
      if( from.contains( "FRIENDLY HAND" ) ) {
        CardPlayed( PLAYER_SELF, cardId.toStdString() );
      } else if( from.contains( "OPPOSING HAND" ) ) {
        CardPlayed( PLAYER_OPPONENT, cardId.toStdString() );
      }
    }

    if( from.contains( "FRIENDLY PLAY" ) && to.contains( "FRIENDLY HAND" ) ) {
      CardReturned( PLAYER_SELF, cardId.toStdString() );
    }

    DEBUG( "Card %s from %s -> %s. (draw: %d, mulligan %d, discard %d)", cardId.toStdString().c_str(), from.toStdString().c_str(), to.toStdString().c_str(), draw, mulligan, discard );
  }

  // Outcome
  QRegExp regexOutcome( "\\[Asset\\].*name=(victory|defeat)_screen_start" );
  if( regexOutcome.indexIn(line) != -1 ) {
    QStringList captures = regexOutcome.capturedTexts();
    QString outcome = captures[1];

    if( outcome == "victory" ) {
      emit HandleOutcome( OUTCOME_VICTORY );
    } else if( outcome == "defeat" ) {
      emit HandleOutcome( OUTCOME_DEFEAT );
    }
    emit HandleMatchEnd( mCardHistoryList );
    Reset();
  }

  // Coin
  QRegExp regexCoin( "ProcessChanges.*zonePos=5.*zone from  -> (.*)" );  // unique because from is nothing -> " "
  if( regexCoin.indexIn(line) != -1 ) {
    QStringList captures = regexCoin.capturedTexts();
    QString to = captures[1];

    if( to.contains( "FRIENDLY HAND" ) ) {
      // I go second because I get the coin
      emit HandleOrder( ORDER_SECOND );
    } else if( to.contains( "OPPOSING HAND" ) ) {
      // Opponent got coin, so I go first
      emit HandleOrder( ORDER_FIRST );
    }
  }

  // Turn Info
  QRegExp regexTurn( "change=powerTask.*tag=NEXT_STEP value=MAIN_ACTION" );
  if( regexTurn.indexIn(line) != -1 ) {
    mTurnCounter++;

    // reset hero power usage on turn change
    mHeroPowerUsed = false;
  }

  // Hero Power
  QRegExp regexHeroPowerEquip( "\\[Zone\\].*player=(\\d+).*-> FRIENDLY PLAY \\(Hero Power\\)" );
  if( regexHeroPowerEquip.indexIn(line) != -1 ) {
    QStringList captures = regexHeroPowerEquip.capturedTexts();
    QString playerId = captures[1];

    mHeroPlayerId = playerId.toInt();
    DEBUG( "Hero Power Equip -> My Player Id: %d", mHeroPlayerId );
  }

  QRegExp regexHeroPower( "\\[Power\\].*cardId=(\\w+).*player=(\\d+)" );
  if( regexHeroPower.indexIn(line) != -1 ) {
    QStringList captures = regexHeroPower.capturedTexts();
    QString cardId = captures[1];
    int playerId = captures[2].toInt();
    Player player = ( playerId == mHeroPlayerId ) ? PLAYER_SELF : PLAYER_OPPONENT;

    bool isHeroPower = false;
    for( int i = 0; i < NUM_HERO_POWER_CARDS; i++ ) {
      if( cardId == HERO_POWER_CARD_IDS[ i ] ) {
        isHeroPower = true;
        break;
      }
    }

    // Power log line is emitted twice
    // Make sure we only account for first occurrence
    // Plus line is emitted when match starts, so ignore turn 0
    if( isHeroPower && !mHeroPowerUsed && CurrentTurn() > 0 ) {
      CardPlayed( player, cardId.toStdString() );
      mHeroPowerUsed = true;
    }
  }

  // Hero Equip
  QRegExp regexHeroEquip( "\\[Zone\\].*cardId=(\\w+).*-> (\\w+) PLAY \\(Hero\\)" );
  if( regexHeroEquip.indexIn(line) != -1 ) {
    QStringList captures = regexHeroEquip.capturedTexts();
    QString cardId = captures[1];
    QString type = captures[2];

    // This log line can be emitted when hero swaps (Lord Jaraxxus)
    // So make sure we only account for the "initial" playable heroes
    Class hero = CLASS_UNKNOWN;
    for( int i = 0; i < NUM_HEROES; i++ ) {
      if( cardId == HERO_IDS[ i ] ) {
        hero = ( Class )i;
      }
    }

    // Set solo mode when encountering naxxramas heros
    if( hero == CLASS_UNKNOWN ) {
      if( cardId.startsWith("NAX") ) {
        HandleGameMode( MODE_SOLO_ADVENTURES );
      }
    }

    if( hero != CLASS_UNKNOWN ) {
      if( type == "FRIENDLY" ) {
        emit HandleMatchStart();
        emit HandleOwnClass( hero );
      } else {
        emit HandleOpponentClass( hero );
      }
    }
  }

  // Game Mode
  // Practice, Casual/Ranked, SreenForge
  QRegExp regexMode( "\\[Bob\\] ---(\\w+)---" );
  if( regexMode.indexIn(line) != -1 ) {
    QStringList captures = regexMode.capturedTexts();
    QString screen = captures[1];

    if( screen == "RegisterScreenPractice" ) {
      HandleGameMode( MODE_SOLO_ADVENTURES );
    } else if( screen == "RegisterScreenTourneys") {
      HandleGameMode( MODE_CASUAL ); // or ranked resp.
    } else if( screen == "RegisterScreenForge" ) {
      HandleGameMode( MODE_ARENA );
    } else if( screen == "RegisterScreenFriendly" ) {
      HandleGameMode( MODE_FRIENDLY );
    }
  }

  // Ranked level
  QRegExp regexRankedLevel( "name=Medal_Ranked_(\\d+)" );
  if( regexRankedLevel.indexIn(line) != -1 ) {
    QStringList captures = regexRankedLevel.capturedTexts();
    QString rank = captures[1];
    HandleRankedLevel(rank.toInt());    
  }

  // Casual/Ranked distinction
  QRegExp regexRanked( "name=rank_window" );
  if( regexRanked.indexIn(line) != -1 ) {
    HandleGameMode( MODE_RANKED );
  }
}

void HearthstoneLogTracker::CardPlayed( Player player, const string& cardId ) {
  DEBUG( "%s played card %s on turn %d", PLAYER_NAMES[ player ], cardId.c_str(), CurrentTurn() );
  mCardHistoryList.push_back( CardHistoryItem( CurrentTurn(), player, cardId ) );
}

void HearthstoneLogTracker::CardReturned( Player player, const string& cardId ) {
  DEBUG( "Card returned %s on turn %d: %s", PLAYER_NAMES[ player ], CurrentTurn(), cardId.c_str() );
  // Make sure we remove the "Choose One"-cards from the history
  // if we decide to withdraw them after a second of thought
  if( !mCardHistoryList.empty() &&
      mCardHistoryList.back().turn == CurrentTurn() &&
      mCardHistoryList.back().player == player &&
      mCardHistoryList.back().cardId == cardId )
  {
    mCardHistoryList.pop_back();
  }
}

int HearthstoneLogTracker::CurrentTurn() const {
  return ( mTurnCounter + 1 ) / 2;
}
