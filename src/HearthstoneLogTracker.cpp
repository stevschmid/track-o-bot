#include "HearthstoneLogTracker.h"
#include "Hearthstone.h"
#include "Settings.h"

#include <QRegExp>
#include <QStringList>
#include <QTimer>
#include <QDir>

// Hero Power Card Ids: Auto generated
const int NUM_HERO_POWER_CARDS = 115;
const char HERO_POWER_CARD_IDS[NUM_HERO_POWER_CARDS][32] = {
"CS1h_001" /* Lesser Heal */, "CS2_017" /* Shapeshift */, "CS2_034" /* Fireblast */, "CS2_049" /* Totemic Call */, "CS2_056" /* Life Tap */, "CS2_083b" /* Dagger Mastery */, "CS2_101" /* Reinforce */, "CS2_102" /* Armor Up! */, "DS1h_292" /* Steady Shot */, "EX1_625t" /* Mind Spike */, "EX1_625t2" /* Mind Shatter */, "EX1_tk33" /* INFERNO! */, "CS2_034_H1" /* Fireblast */, "CS2_034_H1_AT_132" /* Fireblast Rank 2 */, "CS2_101_H1" /* Reinforce */, "CS2_102_H1" /* Armor Up! */, "CS2_102_H1_AT_132" /* Tank Up! */, "DS1h_292_H1" /* Steady Shot */, "DS1h_292_H1_AT_132" /* Ballista Shot */, "TBA01_5" /* Wild Magic */, "TBA01_6" /* Molten Rage */, "TU4d_003" /* Shotgun Blast */, "TU4e_002" /* Flames of Azzinoth */, "NAX10_03" /* Hateful Strike */, "NAX10_03H" /* Hateful Strike */, "NAX11_02" /* Poison Cloud */, "NAX11_02H" /* Poison Cloud */, "NAX12_02" /* Decimate */, "NAX12_02H" /* Decimate */, "NAX13_02" /* Polarity Shift */, "NAX14_02" /* Frost Breath */, "NAX15_02" /* Frost Blast */, "NAX15_02H" /* Frost Blast */, "NAX15_04" /* Chains */, "NAX15_04H" /* Chains */, "NAX1_04" /* Skitter */, "NAX1h_04" /* Skitter */, "NAX2_03" /* Rain of Fire */, "NAX2_03H" /* Rain of Fire */, "NAX3_02" /* Web Wrap */, "NAX3_02H" /* Web Wrap */, "NAX4_04" /* Raise Dead */, "NAX4_04H" /* Raise Dead */, "NAX5_02" /* Eruption */, "NAX5_02H" /* Eruption */, "NAX6_02" /* Necrotic Aura */, "NAX6_02H" /* Necrotic Aura */, "NAX7_03" /* Unbalancing Strike */, "NAX7_03H" /* Unbalancing Strike */, "NAX8_02" /* Harvest */, "NAX8_02H" /* Harvest */, "NAX9_06" /* Unholy Shadow */, "BRMA01_2" /* Pile On! */, "BRMA01_2H" /* Pile On! */, "BRMA02_2" /* Jeering Crowd */, "BRMA02_2H" /* Jeering Crowd */, "BRMA03_2" /* Power of the Firelord */, "BRMA04_2" /* Magma Pulse */, "BRMA05_2" /* Ignite Mana */, "BRMA05_2H" /* Ignite Mana */, "BRMA06_2" /* The Majordomo */, "BRMA06_2H" /* The Majordomo */, "BRMA07_2" /* ME SMASH */, "BRMA07_2H" /* ME SMASH */, "BRMA08_2" /* Intense Gaze */, "BRMA08_2H" /* Intense Gaze */, "BRMA09_2" /* Open the Gates */, "BRMA09_2H" /* Open the Gates */, "BRMA09_3" /* Old Horde */, "BRMA09_3H" /* Old Horde */, "BRMA09_4" /* Blackwing */, "BRMA09_4H" /* Blackwing */, "BRMA09_5" /* Dismount */, "BRMA09_5H" /* Dismount */, "BRMA10_3" /* The Rookery */, "BRMA10_3H" /* The Rookery */, "BRMA11_2" /* Essence of the Red */, "BRMA11_2H" /* Essence of the Red */, "BRMA12_10" /* Mutation */, "BRMA12_2" /* Brood Affliction */, "BRMA12_2H" /* Brood Affliction */, "BRMA13_2" /* True Form */, "BRMA13_2H" /* True Form */, "BRMA13_4" /* Wild Magic */, "BRMA13_4H" /* Wild Magic */, "BRMA14_10" /* Activate! */, "BRMA14_10H" /* Activate! */, "BRMA14_2" /* Activate Arcanotron */, "BRMA14_2H" /* Activate Arcanotron */, "BRMA14_4" /* Activate Toxitron */, "BRMA14_4H" /* Activate Toxitron */, "BRMA14_6" /* Activate Electron */, "BRMA14_6H" /* Activate Electron */, "BRMA14_8" /* Activate Magmatron */, "BRMA14_8H" /* Activate Magmatron */, "BRMA15_2" /* The Alchemist */, "BRMA15_2H" /* The Alchemist */, "BRMA16_2" /* Echolocate */, "BRMA16_2H" /* Echolocate */, "BRMA17_5" /* Bone Minions */, "BRMA17_5H" /* Bone Minions */, "BRMA17_8" /* Nefarian Strikes! */, "BRMA17_8H" /* Nefarian Strikes! */, "BRM_027p" /* DIE, INSECT! */, "BRM_027pH" /* DIE, INSECTS! */, "AT_050t" /* Lightning Jolt */, "AT_132_DRUID" /* Dire Shapeshift */, "AT_132_HUNTER" /* Ballista Shot */, "AT_132_MAGE" /* Fireblast Rank 2 */, "AT_132_PALADIN" /* The Silver Hand */, "AT_132_PRIEST" /* Heal */, "AT_132_ROGUE" /* Poisoned Daggers */, "AT_132_SHAMAN" /* Totemic Slam */, "AT_132_WARLOCK" /* Soul Tap */, "AT_132_WARRIOR" /* Tank Up! */
};

// Hero Ids: Must match HeroClass enum
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

HearthstoneLogTracker::HearthstoneLogTracker( QObject *parent )
  : QObject( parent ), mTurn( 0 ), mHeroPlayerId( 0 ), mLegendTracked( false ), mMatchConcluded( false )
{
  qRegisterMetaType< ::CardHistoryList >( "CardHistoryList" );

  QString hsPath = Settings::Instance()->HearthstoneDirectoryPath();
  QString logFolderPath = QString( "%1/Logs" ).arg( hsPath );

  if( !QDir( logFolderPath ).exists() ) {
    LOG( "Log folder does not exist! Make sure you select the correct Hearthstone path in the settings." );
  } else {
    LOG( "Watching HS logs at %s", qt2cstr( logFolderPath ) );
  }

  for( int i = 0; i < NUM_LOG_MODULES; i++ ) {
    const char *moduleName = LOG_MODULE_NAMES[ i ];

    QString logFilePath = QString( "%1/%2.log" ).arg( logFolderPath ).arg( moduleName );

    HearthstoneLogWatcher *logWatcher = new HearthstoneLogWatcher( this, logFilePath );
    connect( logWatcher, &HearthstoneLogWatcher::LineAdded, this, &HearthstoneLogTracker::HandleLogLine );
    mLogWatchers.push_back( logWatcher );
  }

  Reset();
}

void HearthstoneLogTracker::Reset() {
  mTurn = 0;
  mLegendTracked = false;
  mEntityIdByName.clear();
  mMatchConcluded = false;

  mCardHistoryList.clear();
  emit HandleCardHistoryListUpdate( mCardHistoryList );

  mCardDrawHistoryList.clear();
  emit HandleCardDrawHistoryListUpdate( mCardDrawHistoryList );
}

void HearthstoneLogTracker::HandleLogLine( const QString& line ) {
  if( line.trimmed().isEmpty() || line.startsWith( "(Filename:" )  ) {
    return;
  }

  // LoadingScreen
  static QRegExp regexLoadingScreen( "LoadingScreen.OnSceneLoaded\\(\\) - prevMode=(\\w+) currMode=(\\w+)" );
  if( regexLoadingScreen.indexIn(line) != -1 ) {
    QStringList captures = regexLoadingScreen.capturedTexts();
    QString prevMode = captures[1];
    QString currMode = captures[2];

    // We delay the scene changes to allow some log events to catch up
    // For example the rank mode distinction is only possible
    // via asset unload function, which is triggered on the scene change
    QTimer::singleShot( 1000, [this, prevMode, currMode]() {
      // First check if match concluded for current game mode
      if( prevMode == "GAMEPLAY" && mMatchConcluded ) {
        emit HandleMatchEnd();
        Reset();
      }

      // Then set the new game mode
      if( currMode == "ADVENTURE" ) {
        emit HandleGameMode( MODE_SOLO_ADVENTURES );
      } else if( currMode == "TAVERN_BRAWL" ) {
        emit HandleGameMode( MODE_TAVERN_BRAWL) ;
      } else if( currMode == "DRAFT" ) {
        emit HandleGameMode( MODE_ARENA );
      } else if( currMode == "FRIENDLY" ) {
        emit HandleGameMode( MODE_FRIENDLY );
      } else if( currMode == "TOURNAMENT" ) {
        // casual or ranked
        emit HandleGameMode( MODE_CASUAL );
      }

      DBG( "Switch scene from %s to %s", qt2cstr( prevMode ), qt2cstr( currMode ) );
    });
  }

  // Coin
  static QRegExp regexCoin( "ZoneChangeList.ProcessChanges.*local=False.*zonePos=5.*zone from  -> (.*)" );
  if( regexCoin.indexIn(line) != -1 ) {
    QStringList captures = regexCoin.capturedTexts();
    QString to = captures[1];

    if( CurrentTurn() == 0 ) {
      if( to.contains( "FRIENDLY HAND" ) ) {
        // I go second because I get the coin
        emit HandleOrder( ORDER_SECOND );
      } else if( to.contains( "OPPOSING HAND" ) ) {
        // Opponent got coin, so I go first
        emit HandleOrder( ORDER_FIRST );
      }
    }
  }

  // CardPlayed
  static QRegExp regexCardPlayed( "ZoneChangeList.ProcessChanges.*\\[.*id=(\\d+).*zone=(\\w+).*cardId=(\\w+).*\\].*zone from (.*) ->\\s?(.*)" );
  if( regexCardPlayed.indexIn(line) != -1 ) {
    QStringList captures = regexCardPlayed.capturedTexts();
    int id = captures[1].toInt();
    QString zone = captures[2];
    QString cardId = captures[3];
    QString from = captures[4];
    QString to = captures[5];

    bool draw = (from.isEmpty() || from.contains( "DECK" )) && to.contains( "HAND" );
    bool mulligan = from.contains( "HAND" ) && to.contains( "DECK" );

    // Discarded cards by playing Soulfire, Doomguard etc.
    bool discard = from.contains( "HAND" ) && to.contains( "GRAVEYARD" );

    // Set aside cards, i.e. by playing Golden Monkey
    bool setaside = zone.contains( "SETASIDE" );

    if( !draw && !mulligan && !discard && !setaside ) {
      if( from.contains( "FRIENDLY HAND" ) ) {
        CardPlayed( PLAYER_SELF, cardId, id );
      } else if( from.contains( "OPPOSING HAND" ) ) {
        CardPlayed( PLAYER_OPPONENT, cardId, id );
      } else if( from.contains( "OPPOSING SECRET" ) && to.contains( "OPPOSING GRAVEYARD" ) ) {
        SecretResolved( PLAYER_OPPONENT, cardId, id );
      }
    }

    if( draw && to.contains( "FRIENDLY HAND" ) ) {
      CardDrawn( PLAYER_SELF, cardId, id );
    }
    if( mulligan && to.contains( "FRIENDLY DECK" ) ) {
      CardUndrawn( PLAYER_SELF, cardId, id );
    }

    DBG( "Card %s [%d] from %s -> %s. (draw: %d, mulligan %d, discard %d, setaside %d) [%d]", qt2cstr( cardId ), id, qt2cstr( from ), qt2cstr( to ), draw, mulligan, discard, setaside, id );
  }

  // CardReturned
  // Wrath: "from  -> FRIENDLY HAND"
  // DotClaw: "from FRIENDLY PLAY -> FRIENDLY HAND"
  static QRegExp regexCardReturned( "ZoneChangeList.ProcessChanges.*local=True.*cardId=(\\w+).*zone from (FRIENDLY PLAY)? -> FRIENDLY HAND" );
  if( regexCardReturned.indexIn(line) != -1 ) {
    QStringList captures = regexCardReturned.capturedTexts();
    QString cardId = captures[1];
    CardReturned( PLAYER_SELF, cardId );
  }

  // Entity
  static QRegExp regexPlayerEntity( "PowerTaskList.DebugPrintPower.*TAG_CHANGE Entity=(.+) tag=PLAYER_ID value=(\\d+)" );
  regexPlayerEntity.setMinimal( true ); // non-greedy for entity name
  if( regexPlayerEntity.indexIn(line) != -1 ) {
    QStringList captures = regexPlayerEntity.capturedTexts();
    QString entityName = captures[1];
    int entityId = captures[2].toInt();

    mEntityIdByName[ entityName ] = entityId;
    DBG( "Player Entity %s = %d", qt2cstr( entityName ), entityId );
  }

  // Start
  static QRegExp regexStart( "PowerTaskList.DebugPrintPower.*CREATE_GAME");
  if( regexStart.indexIn(line) != -1 ) {
    DBG( "Create game" );
    emit HandleMatchStart();
  }

  // End
  static QRegExp regexEnd( "PowerTaskList.DebugPrintPower.*TAG_CHANGE Entity=(.+) tag=PLAYSTATE value=(WON|LOST|TIED)" );
  regexEnd.setMinimal( true ); // non-greedy for entity name
  if( regexEnd.indexIn(line) != -1 ) {
    QStringList captures = regexEnd.capturedTexts();
    QString entityName = captures[1];
    QString outcome = captures[2];

    int entityId = mEntityIdByName.value( entityName, -1 );
    if( entityId == -1 ) {
      LOG( "Could not resolve entity %s to determine outcome", qt2cstr( entityName ) );
    } else {
      if( entityId == mHeroPlayerId ) {
        if( outcome == "LOST" ) {
          emit HandleOutcome( OUTCOME_DEFEAT );
        } else {
          // WON and TIED
          emit HandleOutcome( OUTCOME_VICTORY );
        }
      }
    }

    mMatchConcluded = true;
  }

  // Turn Info
  static QRegExp regexTurn( "PowerTaskList.DebugPrintPower.*TAG_CHANGE Entity=GameEntity tag=TURN value=(\\d+)" );
  if( regexTurn.indexIn(line) != -1 ) {
    QStringList captures = regexTurn.capturedTexts();
    mTurn = captures[1].toInt();
    emit HandleTurn( mTurn );
  }

  // Hero Power
  static QRegExp regexHeroPowerEquip( "ZoneChangeList.ProcessChanges.*player=(\\d+).*-> FRIENDLY PLAY \\(Hero Power\\)" );
  if( regexHeroPowerEquip.indexIn(line) != -1 ) {
    QStringList captures = regexHeroPowerEquip.capturedTexts();
    QString playerId = captures[1];

    mHeroPlayerId = playerId.toInt();
    DBG( "Hero Power Equip -> My Player Id: %d", mHeroPlayerId );
  }

  static QRegExp regexHeroPower( "PowerTaskList.DebugPrintPower.*ACTION_START.*zone=PLAY zonePos=0 cardId=(\\w+) player=(\\d+).*BlockType=POWER" );
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
    if( isHeroPower ) {
      CardPlayed( player, cardId );
    }
  }

  // Hero Equip
  static QRegExp regexHeroEquip( "ZoneChangeList.ProcessChanges.*cardId=(\\w+).*-> (\\w+) PLAY \\(Hero\\)" );
  if( regexHeroEquip.indexIn(line) != -1 ) {
    QStringList captures = regexHeroEquip.capturedTexts();
    QString cardId = captures[1];
    QString type = captures[2];

    // This log line can be emitted when hero swaps (Lord Jaraxxus)
    // So make sure we only account for the "initial" playable heroes
    HeroClass hero = CLASS_UNKNOWN;
    for( int i = 0; i < NUM_HEROES; i++ ) {
      // startsWith instead of exact match to support
      // the new reasonably priced hero skins
      // (e.g. HERO_01a instead of HERO_01)
      if( cardId.startsWith( HERO_IDS[ i ] ) ) {
        hero = ( HeroClass )i;
      }
    }

    if( hero != CLASS_UNKNOWN ) {
      if( type == "FRIENDLY" ) {
        emit HandleOwnClass( hero );
      } else {
        emit HandleOpponentClass( hero );
      }
    }
  }

  // Rank
  // Rank events via log are unreliable

  // Legend
  // Emitted at the end of the game twice, make sure we capture only the first time
  static QRegExp regexLegend( "legend rank (\\d+)" );
  if( !mLegendTracked && regexLegend.indexIn(line) != -1 ) {
    QStringList captures = regexLegend.capturedTexts();
    int legend = captures[1].toInt();
    if( legend > 0 ) {
      mLegendTracked = true;
      emit HandleLegend( legend );
    }
  }

  // Casual/Ranked distinction
  // This is a Unloading Asset event, which may be unreliable in the timing
  // Alas, I don't see a more reliable way to distinguish casual from ranked
  static QRegExp regexRanked( "name=rank_window" );
  if( regexRanked.indexIn(line) != -1 ) {
    DBG( "Detected ranked game" );
    emit HandleGameMode( MODE_RANKED );
  }

  // flag current GAME as spectated
  static QRegExp regexBeginSpectating( "Start Spectator Game" );
  if( regexBeginSpectating.indexIn(line) != -1 ) {
    DBG( "Begin spectator game" );
    emit HandleSpectating( true );
  }

  // disable spectating flag if we leave the spectator MODE
  static QRegExp regexEndSpectating( "End Spectator Mode" );
  if( regexEndSpectating.indexIn(line) != -1 ) {
    DBG( "End spectator mode" );
    emit HandleSpectating( false );
  }
}

void HearthstoneLogTracker::CardPlayed( Player player, const QString& cardId, int internalId ) {
  DBG( "%s played card %s on turn %d", PLAYER_NAMES[ player ], qt2cstr( cardId ), CurrentTurn() );

  mCardHistoryList.push_back( CardHistoryItem( CurrentTurn(), player, cardId, internalId ) );
  emit HandleCardHistoryListUpdate( mCardHistoryList );
}

void HearthstoneLogTracker::CardReturned( Player player, const QString& cardId ) {
  DBG( "Card returned %s on turn %d: %s", PLAYER_NAMES[ player ], CurrentTurn(), qt2cstr( cardId ) );
  // Make sure we remove the "Choose One"-cards from the history
  // if we decide to withdraw them after a second of thought
  if( !mCardHistoryList.empty() &&
      mCardHistoryList.back().turn == CurrentTurn() &&
      mCardHistoryList.back().player == player &&
      mCardHistoryList.back().cardId == cardId )
  {
    mCardHistoryList.pop_back();
    emit HandleCardHistoryListUpdate( mCardHistoryList );
  }
}

void HearthstoneLogTracker::CardDrawn( Player player, const QString& cardId, int internalId ) {
  DBG( "%s Card drawn %s on turn %d (%d)", PLAYER_NAMES[ player ], qt2cstr( cardId ), CurrentTurn(), internalId );

  mCardDrawHistoryList.push_back( CardHistoryItem( CurrentTurn(), player, cardId, internalId ) ) ;
  emit HandleCardDrawHistoryListUpdate( mCardDrawHistoryList );
}

void HearthstoneLogTracker::CardUndrawn( Player player, const QString& cardId, int internalId ) {
  DBG( "%s Card undrawn %s on turn %d (%d)", PLAYER_NAMES[ player ], qt2cstr( cardId ), CurrentTurn(), internalId );

  CardHistoryList::iterator it = mCardDrawHistoryList.begin();
  while( it != mCardDrawHistoryList.end() ) {
    if( (*it).internalId == internalId && (*it).player == player )
    {
      it = mCardDrawHistoryList.erase( it );
      emit HandleCardDrawHistoryListUpdate( mCardDrawHistoryList );
    } else {
      it++;
    }
  }
}

void HearthstoneLogTracker::SecretResolved( Player player, const QString& cardId, int internalId ) {
  DBG( "Secret resolved by %s: %s", PLAYER_NAMES[ player ], qt2cstr( cardId ) );
  for( CardHistoryItem& item : mCardHistoryList ) {
    if( item.player == player && item.internalId == internalId ) {
      item.cardId = cardId;
    }
  }
}

int HearthstoneLogTracker::CurrentTurn() const {
  return ( mTurn + 1 ) / 2;
}
