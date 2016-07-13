#include "HearthstoneLogTracker.h"
#include "Hearthstone.h"
#include "Settings.h"

#include <QRegExp>
#include <QRegularExpression>
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
  : QObject( parent ), mTurn( 0 ), mHeroPlayerId( 0 ), mLegendTracked( false )
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

    HearthstoneLogWatcher *logWatcher = new HearthstoneLogWatcher( this, moduleName, logFilePath );
    connect( logWatcher, &HearthstoneLogWatcher::LineAdded, this, &HearthstoneLogTracker::HandleLogLine );
    mLogWatchers.push_back( logWatcher );
  }

  Reset();

  // Add handlers
  RegisterHearthstoneLogLineHandler( "LoadingScreen", "LoadingScreen.OnSceneLoaded()", "prevMode=(?<prevMode>\\w+) currMode=(?<currMode>\\w+)", &HearthstoneLogTracker::OnSceneLoaded );
  RegisterHearthstoneLogLineHandler( "Zone", "ZoneChangeList.ProcessChanges()", "local=(?<local>\\w+) (?<entity>\\[.+?\\]) zone from (?<from>.*) ->\\s?(?<to>.*)", &HearthstoneLogTracker::OnZoneChange );
  RegisterHearthstoneLogLineHandler( "Power", "PowerTaskList.DebugPrintPower()", "TAG_CHANGE Entity=(?<entity>.+?) tag=(?<tag>\\w+) value=(?<value>\\w+)", &HearthstoneLogTracker::OnTagChange );
  RegisterHearthstoneLogLineHandler( "Power", "PowerTaskList.DebugPrintPower()", "CREATE_GAME", &HearthstoneLogTracker::OnCreateGame );
  RegisterHearthstoneLogLineHandler( "Power", "PowerTaskList.DebugPrintPower()", "BLOCK_START BlockType=(?<blockType>.+?) Entity=(?<entity>.+?) EffectCardId=", &HearthstoneLogTracker::OnActionStart );
  RegisterHearthstoneLogLineHandler( "Power", "GameState.DebugPrintEntityChoices()", "id=\\d+ Player=(?<name>.+?) TaskList=", &HearthstoneLogTracker::OnPlayerName );
  RegisterHearthstoneLogLineHandler( "Power", "GameState.DebugPrintEntityChoices()", "type=INVALID zone=DECK zonePos=0 player=(?<id>\\d+)", &HearthstoneLogTracker::OnPlayerId );
  RegisterHearthstoneLogLineHandler( "Bob", "", "legend rank (?<rank>\\w+)", &HearthstoneLogTracker::OnLegendRank );
  RegisterHearthstoneLogLineHandler( "Asset", "", "name=rank_window", &HearthstoneLogTracker::OnRanked );
  RegisterHearthstoneLogLineHandler( "Power", "", "Start Spectator Game", &HearthstoneLogTracker::OnStartSpectating );
  RegisterHearthstoneLogLineHandler( "Power", "", "End Spectator Mode", &HearthstoneLogTracker::OnStopSpectating ); // MODE!
}


void HearthstoneLogTracker::OnPlayerName( const QVariantMap& args ) {
  mCurrentPlayerName = args[ "name" ].toString();
  DBG( "OnPlayerName %s", qt2cstr( mCurrentPlayerName ) );
}

void HearthstoneLogTracker::OnPlayerId( const QVariantMap& args ) {
  int id = args[ "id" ].toInt();

  DBG( "OnPlayerId %d. Set %s to id %d", id, qt2cstr( mCurrentPlayerName ), id );
  mPlayerIdsByName[ mCurrentPlayerName ] = id;
}

void HearthstoneLogTracker::OnActionStart( const QVariantMap& args ) {
  QString blockType = args[ "blockType" ].toString();

  QVariantMap entity = args[ "entity" ].toMap();
  QString cardId = entity[ "cardId" ].toString();
  int playerId = entity[ "player" ].toInt();

  DBG( "OnActionStart %s %s %d", qt2cstr( blockType ), qt2cstr( cardId ), playerId );

  if( blockType == "POWER" ) {
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
}

void HearthstoneLogTracker::OnCreateGame( const QVariantMap& args ) {
  UNUSED_ARG( args );

  DBG( "OnCreateGame" );
  emit HandleMatchStart();
}


void HearthstoneLogTracker::OnLegendRank( const QVariantMap& args ) {
  // Legend
  // Emitted at the end of the game twice, make sure we capture only the first time
  int legend = args[ "rank" ].toInt();
  DBG( "OnLegendRank %d", legend );
  if( legend > 0 ) {
    mLegendTracked = true;
    emit HandleLegend( legend );
  }
}

void HearthstoneLogTracker::OnRanked( const QVariantMap& args ) {
  UNUSED_ARG( args );

  DBG( "OnRanked" );

  // Casual/Ranked distinction
  // This is a Unloading Asset event, which may be unreliable in the timing
  // Alas, I don't see a more reliable way to distinguish casual from ranked
  emit HandleGameMode( MODE_RANKED );
}

void HearthstoneLogTracker::OnSceneLoaded( const QVariantMap& args ) {
  QString prevMode = args[ "prevMode" ].toString();
  QString currMode = args[ "currMode" ].toString();

  DBG( "OnSceneLoaded %s -> %s", qt2cstr( prevMode ), qt2cstr( currMode ) );

  // We delay the scene changes to allow some log events to catch up
  // For example the rank mode distinction is only possible
  // via asset unload function, which is triggered on the scene change
  QTimer::singleShot( 2500, [this, prevMode, currMode]() {
    // First check if match concluded for current game mode
    if( prevMode == "GAMEPLAY" ) {
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

void HearthstoneLogTracker::OnStartSpectating( const QVariantMap& args ) {
  UNUSED_ARG( args );

  // flag current GAME as spectated
  DBG( "OnStartSpectating" );
  emit HandleSpectating( true );
}

void HearthstoneLogTracker::OnStopSpectating( const QVariantMap& args ) {
  UNUSED_ARG( args );

  // disable spectating flag if we leave the spectator MODE
  DBG( "OnStopSpectating" );
  emit HandleSpectating( false );

  Reset();
}

void HearthstoneLogTracker::OnTagChange( const QVariantMap& args ) {
  QString tag = args[ "tag" ].toString();
  QString value = args[ "value" ].toString();

  DBG( "OnTagChange %s = %s", qt2cstr( tag ), qt2cstr( value ) );

  if( tag == "PLAYSTATE" && ( value == "WON" || value == "LOST" || value == "TIED" ) ) {
    QString entityName = args[ "entity" ].toString();
    QString outcome = value;

    int entityId = mPlayerIdsByName.value( entityName, -1 );
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
  }

  if( tag == "TURN" ) {
    mTurn = value.toInt();
    emit HandleTurn( mTurn );
  }
}

void HearthstoneLogTracker::OnZoneChange( const QVariantMap& args ) {
  QString from = args[ "from" ].toString();
  QString to = args[ "to" ].toString();
  bool local = args[ "local" ].toBool();

  QVariantMap entity = args[ "entity" ].toMap();
  int id = entity[ "id" ].toInt();
  QString zone = entity[ "zone" ].toString();
  QString cardId = entity[ "cardId" ].toString();
  int playerId = entity[ "player" ].toInt();

  Player player = from.contains( "FRIENDLY" ) || to.contains( "FRIENDLY" ) ? PLAYER_SELF : PLAYER_OPPONENT;

  DBG( "OnZoneChange %s -> %s (entity id %d)", qt2cstr( from ), qt2cstr( to ), id );

  /*
   * The Coin
   */
  if( entity[ "zonePos" ] == 5 && from.isEmpty() && CurrentTurn() <= 1 ) {
    if( to.contains( "FRIENDLY HAND" ) ) {
      // I go second because I get the coin
      emit HandleOrder( ORDER_SECOND );
    } else if( to.contains( "OPPOSING HAND" ) ) {
      // Opponent got coin, so I go first
      emit HandleOrder( ORDER_FIRST );
    }
  }

  if( CurrentTurn() == 0 && from.isEmpty() && to.contains( "DECK" ) ) {
    // Since HS "creates" deck cards on the fly for events such as jousting or elekk
    // Keep track of those initial cards
    mInitialDeckObjectIds.push_back( id );
  }

  /*
   * Cards played/drawn
   */
  // Cases to consider:
  // Initial cards, Mulligan, Mill, Joust, Malorne, Deathlord, Voidwalker, Discard, Tracking

  // Card played?
  // "": spell, PLAY: minion, weapon, SECRET: secret
  bool playedFromHand = from.contains( "HAND" ) && ( to.isEmpty() || to.contains( "PLAY" ) || to.contains( "SECRET" ) );

  // I.e. by deathlord
  bool playedFromDeck = from.contains( "DECK" ) && ( to.contains( "PLAY" ) || to.contains( "SECRET" ) );

  // Set aside cards, i.e. by playing Golden Monkey or tracking
  bool setaside = zone.contains( "SETASIDE" );

  // Card drawn?
  // "" && turn = 0: initial draw, DECK: remaining draws (anything which comes from the deck)
  bool draw = ( from.isEmpty() && CurrentTurn() == 0 && to.contains("HAND") ) || // starting hand
              ( from.contains( "DECK" ) && to.isEmpty() && mInitialDeckObjectIds.contains( id ) ) || // e.g. tracking
              ( from.contains( "DECK") && !to.isEmpty() ); // normal draw


  // Card put back? (i.e. mulligan)
  // GRAVEYARD (malorne)
  bool putBackToDeck = ( from.contains( "HAND" ) || from.contains( "GRAVEYARD" ) ) && to.contains( "DECK" );

  // Play cancelled?
  // Wrath: "from  -> FRIENDLY HAND"
  // DotC: "from FRIENDLY PLAY -> FRIENDLY HAND"
  bool playCancelled = local && ( from.isEmpty() || from.contains( "PLAY" ) ) && to.contains( "HAND" );

  if( draw ) {
    CardDrawn( player, cardId, id );
  } else if( putBackToDeck ) {
    CardUndrawn( player, cardId, id );
  }

  if( !setaside ) {
    if( playedFromHand || playedFromDeck ) {
      CardPlayed( player, cardId, id );
    } else if( playCancelled ) {
      CardReturned( player, cardId, id );
    }
  }

  if( !cardId.isEmpty() ) {
    // When secrets get resolved, cards discarded etc. update the internal id to the revealed card id
    ResolveCard( player, cardId, id );
  }

  /*
   * Hero Equip
   */
  if( to.contains( "PLAY (Hero)" ) ) {
    // This can happen when hero swaps (Lord Jaraxxus)
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
      if( player == PLAYER_SELF ) {
        emit HandleOwnClass( hero );
      } else {
        emit HandleOpponentClass( hero );
      }
    }
  }

  /*
   * Use Hero Power Equip to find ids for mapping players
   */
  if( to.contains( "FRIENDLY PLAY (Hero Power)" ) ) {
    mHeroPlayerId = playerId;
  }
}

void HearthstoneLogTracker::RegisterHearthstoneLogLineHandler( const QString& module, const QString& call, const QString& regex, void (HearthstoneLogTracker::*func)( const QVariantMap& args ) ) {
  HearthstoneLogLineHandler *handler = new HearthstoneLogLineHandler( this, module, call, regex );
  connect( handler, &HearthstoneLogLineHandler::Handle, this, func );
  mLineHandlers << handler;
}

void HearthstoneLogTracker::Reset() {
  DBG( "Reset" );

  mTurn = 0;
  mLegendTracked = false;
  mPlayerIdsByName.clear();
  mInitialDeckObjectIds.clear();

  mCardsPlayed.clear();
  emit HandleCardsPlayedUpdate( mCardsPlayed );

  mCardsDrawn.clear();
  emit HandleCardsDrawnUpdate( mCardsDrawn );
}

void HearthstoneLogTracker::HandleLogLine( const QString& module, const QString& line ) {
  if( line.trimmed().isEmpty() || line.startsWith( "(Filename:" ) ) {
    return;
  }

  for( HearthstoneLogLineHandler* lineHandler : mLineHandlers ) {
    lineHandler->Process( module, line );
  }
}

void HearthstoneLogTracker::CardPlayed( Player player, const QString& cardId, int internalId ) {
  DBG( "%s played card %s on turn %d (id %d)", PLAYER_NAMES[ player ], qt2cstr( cardId ), CurrentTurn(), internalId );

  mCardsPlayed.push_back( CardHistoryItem( CurrentTurn(), player, cardId, internalId ) );
  emit HandleCardsPlayedUpdate( mCardsPlayed );
}

void HearthstoneLogTracker::CardReturned( Player player, const QString& cardId, int internalId ) {
  DBG( "%s returned card %s on turn %d (id %d)", PLAYER_NAMES[ player ], qt2cstr( cardId ), CurrentTurn(), internalId );

  // Make sure we remove the "Choose One"-cards from the history
  // if we decide to withdraw them after a second of thought
  if( !mCardsPlayed.empty() && mCardsPlayed.back().internalId == internalId ) {
    mCardsPlayed.pop_back();
    emit HandleCardsPlayedUpdate( mCardsPlayed );
  }
}

void HearthstoneLogTracker::CardDrawn( Player player, const QString& cardId, int internalId ) {
  DBG( "%s Card drawn %s on turn %d (%d)", PLAYER_NAMES[ player ], qt2cstr( cardId ), CurrentTurn(), internalId );

  mCardsDrawn.push_back( CardHistoryItem( CurrentTurn(), player, cardId, internalId ) );
  emit HandleCardsDrawnUpdate( mCardsDrawn );
}

void HearthstoneLogTracker::CardUndrawn( Player player, const QString& cardId, int internalId ) {
  DBG( "%s Card undrawn %s on turn %d (%d)", PLAYER_NAMES[ player ], qt2cstr( cardId ), CurrentTurn(), internalId );

  CardHistoryList::iterator it = mCardsDrawn.begin();
  while( it != mCardsDrawn.end() ) {
    if( (*it).internalId == internalId && (*it).player == player ) { // check player too in case of entomb
      it = mCardsDrawn.erase( it );
      emit HandleCardsDrawnUpdate( mCardsDrawn );
    } else {
      it++;
    }
  }
}

void HearthstoneLogTracker::ResolveCard( Player player, const QString& cardId, int internalId ) {
  DBG( "Card %d resolved for %s: %s", internalId, PLAYER_NAMES[ player ], qt2cstr( cardId ) );
  for( CardHistoryItem& item : mCardsPlayed ) {
    if( item.player == player && item.internalId == internalId ) {
      if( item.cardId != cardId ) {
        item.cardId = cardId;
        emit HandleCardsPlayedUpdate( mCardsPlayed );
      }
    }
  }

  for( CardHistoryItem& item : mCardsDrawn ) {
    if( item.player == player && item.internalId == internalId ) {
      if( item.cardId != cardId ) {
        item.cardId = cardId;
        emit HandleCardsDrawnUpdate( mCardsDrawn );
      }
    }
  }
}

int HearthstoneLogTracker::CurrentTurn() const {
  return ( mTurn + 1 ) / 2;
}
