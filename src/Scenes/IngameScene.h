#include "ShiftDetector.h"

// if after X ms the ingame marker is still absent, look for a shift in
// the scene (e.g. caused by a heavy minion or a spell)
#define INGAME_SHIFT_DETECTION_GRACE_PERIOD 3000

class IngameScene : public QObject, public Scene
{
  Q_OBJECT

private:
  bool mOutcomeDetected;
  bool mOrderDetected;
  bool mOwnClassDetected;
  bool mOpponentClassDetected;

  ShiftDetector mShiftDetector;

public:
  IngameScene()
    : Scene( "Ingame" ), mShiftDetector( this, "ingame", INGAME_SHIFT_DETECTION_GRACE_PERIOD )
  {
    ADD_GENERATED_MARKER( "ingame", INGAME_ID );
    ADD_GENERATED_MARKER( "going_first", INGAME_MULLIGAN_1ST_ID );
    ADD_GENERATED_MARKER( "going_second", INGAME_MULLIGAN_2ND_ID );

    ADD_GENERATED_MARKER( "victory", INGAME_VICTORY_ID );
    ADD_GENERATED_MARKER( "defeat", INGAME_DEFEAT_ID );

    ADD_GENERATED_MARKER( "own_class_priest", INGAME_PRIEST_ME );
    ADD_GENERATED_MARKER( "opponent_class_priest", INGAME_PRIEST_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_rogue", INGAME_ROGUE_ME );
    ADD_GENERATED_MARKER( "opponent_class_rogue", INGAME_ROGUE_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_shaman", INGAME_SHAMAN_ME );
    ADD_GENERATED_MARKER( "opponent_class_shaman", INGAME_SHAMAN_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_paladin", INGAME_PALADIN_ME );
    ADD_GENERATED_MARKER( "opponent_class_paladin", INGAME_PALADIN_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_warlock", INGAME_WARLOCK_ME );
    ADD_GENERATED_MARKER( "opponent_class_warlock", INGAME_WARLOCK_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_warrior", INGAME_WARRIOR_ME );
    ADD_GENERATED_MARKER( "opponent_class_warrior", INGAME_WARRIOR_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_mage", INGAME_MAGE_ME );
    ADD_GENERATED_MARKER( "opponent_class_mage", INGAME_MAGE_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_druid", INGAME_DRUID_ME );
    ADD_GENERATED_MARKER( "opponent_class_druid", INGAME_DRUID_OPPONENT );

    ADD_GENERATED_MARKER( "own_class_hunter", INGAME_HUNTER_ME );
    ADD_GENERATED_MARKER( "opponent_class_hunter", INGAME_HUNTER_OPPONENT );

    Reset();
  }

  void Reset() {
    Scene::Reset();

    mOutcomeDetected       = false;
    mOrderDetected         = false;
    mOwnClassDetected      = false;
    mOpponentClassDetected = false;

    mShiftDetector.Reset();
  }

  void Update() {
    mShiftDetector.Update();

    if( !mOrderDetected ) {
      GoingOrder order = ORDER_UNKNOWN;

      if( FindMarker("going_first") ) {
        order = ORDER_FIRST;
      } else if( FindMarker("going_second") ) {
        order = ORDER_SECOND;
      }

      if( order != ORDER_UNKNOWN ) {
        HandleOrder( order );
        mOrderDetected = true;
      }
    }
    if( !mOutcomeDetected ) {
      Outcome outcome = OUTCOME_UNKNOWN;

      if( FindMarker("victory") ) {
        outcome = OUTCOME_VICTORY;
      } else if( FindMarker("defeat") ) {
        outcome = OUTCOME_DEFEAT;
      }

      if( outcome != OUTCOME_UNKNOWN ) {
        HandleOutcome( outcome );
        mOutcomeDetected = true;
      }
    }
    if( !mOwnClassDetected ) {
      Class ownClass = CLASS_UNKNOWN;

      for( int i = 0; i < NUM_CLASSES; i++ ) {
        string className = CLASS_NAMES[ i ];
        if( FindMarker( string( "own_class_" ) + className ) ) {
          ownClass = ( Class )i;
          break;
        }
      }

      if( ownClass != CLASS_UNKNOWN ) {
        HandleOwnClass( ownClass );
        mOwnClassDetected = true;
      }
    }
    if( !mOpponentClassDetected ) {
      Class opponentClass = CLASS_UNKNOWN;

      for( int i = 0; i < NUM_CLASSES; i++ ) {
        string className = CLASS_NAMES[ i ];
        if( FindMarker( string( "opponent_class_" ) + className ) ) {
          opponentClass = ( Class )i;
          break;
        }
      }

      if( opponentClass != CLASS_UNKNOWN ) {
        HandleOpponentClass( opponentClass );
        mOpponentClassDetected = true;
      }
    }
  }

  bool Active() {
    return FindMarker( "ingame" );
  }

signals:
  void HandleOutcome( Outcome outcome );
  void HandleOwnClass( Class ownClass );
  void HandleOpponentClass( Class opponentClass );
  void HandleOrder( GoingOrder order );

};
