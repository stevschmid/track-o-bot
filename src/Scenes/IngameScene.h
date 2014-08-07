#include "ShiftDetector.h"

// if after X ms the ingame marker is still absent, look for a shift in
// the scene (e.g. caused by a heavy minion or a spell)
#define INGAME_SHIFT_DETECTION_GRACE_PERIOD 3000

class IngameScene : public Scene
{
private:
  Outcome      mOutcome;
  GoingOrder   mOrder;
  Class        mOwnClass;
  Class        mOpponentClass;

  ShiftDetector mShiftDetector;

public:
  IngameScene()
    : Scene( "Ingame" ), mShiftDetector( this, "ingame", INGAME_SHIFT_DETECTION_GRACE_PERIOD )
  {
    ADD_GENERATED_MARKER( "ingame", INGAME_ID );
    ADD_GENERATED_MARKER( "going_first", INGAME_MULLIGAN_1ST_ID );
    ADD_GENERATED_MARKER( "going_second", INGAME_MULLIGAN_2ND_ID );

    ADD_GENERATED_MARKER( "victory1", INGAME_VICTORY_ID1 );
    ADD_GENERATED_MARKER( "defeat1", INGAME_DEFEAT_ID1 );

    ADD_GENERATED_MARKER( "victory2", INGAME_VICTORY_ID2 );
    ADD_GENERATED_MARKER( "defeat2", INGAME_DEFEAT_ID2 );

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

    mOutcome       = OUTCOME_UNKNOWN;
    mOrder         = ORDER_UNKNOWN;
    mOwnClass      = CLASS_UNKNOWN;
    mOpponentClass = CLASS_UNKNOWN;

    mShiftDetector.Reset();
  }

  void Update() {
    mShiftDetector.Update();

    if( mOrder == ORDER_UNKNOWN ) {
      if( FindMarker("going_first") ) {
        mOrder = ORDER_FIRST;
      }
      if( FindMarker("going_second") ) {
        mOrder = ORDER_SECOND;
      }
    }
    if( mOutcome == OUTCOME_UNKNOWN ) {
      if( FindMarker("victory1") || FindMarker("victory2") ) {
        mOutcome = OUTCOME_VICTORY;
      }
      if( FindMarker("defeat1") || FindMarker("defeat2") ) {
        mOutcome = OUTCOME_DEFEAT;
      }
    }
    if( mOwnClass == CLASS_UNKNOWN || mOpponentClass == CLASS_UNKNOWN ) {
      for( int i = 0; i < NUM_CLASSES; i++ ) {
        string className = CLASS_NAMES[ i ];
        if( FindMarker( string( "own_class_" ) + className ) ) {
          mOwnClass = ( Class )i;
        }
        if( FindMarker( string( "opponent_class_" ) + className ) ) {
          mOpponentClass = ( Class )i;
        }
      }
    }
  }

  bool Active() {
    return FindMarker( "ingame" );
  }

  Outcome Outcome() const {
    return mOutcome;
  }

  Class OwnClass() const {
    return mOwnClass;
  }

  Class OpponentClass() const {
    return mOpponentClass;
  }

  GoingOrder GoingOrder() const {
    return mOrder;
  }

};
