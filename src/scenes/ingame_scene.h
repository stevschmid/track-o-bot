class IngameScene : public Scene
{
private:
  Outcome outcome;
  GoingOrder order;
  Class ownClass;
  Class opponentClass;

public:
  IngameScene()
    :Scene("Ingame")
  {
    ADD_GENERATED_MARKER("ingame", INGAME_ID);
    ADD_GENERATED_MARKER("going_first", INGAME_MULLIGAN_1ST_ID);
    ADD_GENERATED_MARKER("going_second", INGAME_MULLIGAN_2ND_ID);
    ADD_GENERATED_MARKER("victory", INGAME_VICTORY_ID);
    ADD_GENERATED_MARKER("defeat", INGAME_DEFEAT_ID);

    ADD_GENERATED_MARKER("own_class_priest", INGAME_PRIEST_ME);
    ADD_GENERATED_MARKER("opponent_class_priest", INGAME_PRIEST_OPPONENT);

    ADD_GENERATED_MARKER("own_class_rogue", INGAME_ROGUE_ME);
    ADD_GENERATED_MARKER("opponent_class_rogue", INGAME_ROGUE_OPPONENT);

    ADD_GENERATED_MARKER("own_class_shaman", INGAME_SHAMAN_ME);
    ADD_GENERATED_MARKER("opponent_class_shaman", INGAME_SHAMAN_OPPONENT);

    ADD_GENERATED_MARKER("own_class_paladin", INGAME_PALADIN_ME);
    ADD_GENERATED_MARKER("opponent_class_paladin", INGAME_PALADIN_OPPONENT);

    ADD_GENERATED_MARKER("own_class_warlock", INGAME_WARLOCK_ME);
    ADD_GENERATED_MARKER("opponent_class_warlock", INGAME_WARLOCK_OPPONENT);

    ADD_GENERATED_MARKER("own_class_warrior", INGAME_WARRIOR_ME);
    ADD_GENERATED_MARKER("opponent_class_warrior", INGAME_WARRIOR_OPPONENT);

    ADD_GENERATED_MARKER("own_class_mage", INGAME_MAGE_ME);
    ADD_GENERATED_MARKER("opponent_class_mage", INGAME_MAGE_OPPONENT);

    ADD_GENERATED_MARKER("own_class_druid", INGAME_DRUID_ME);
    ADD_GENERATED_MARKER("opponent_class_druid", INGAME_DRUID_OPPONENT);

    ADD_GENERATED_MARKER("own_class_hunter", INGAME_HUNTER_ME);
    ADD_GENERATED_MARKER("opponent_class_hunter", INGAME_HUNTER_OPPONENT);
  }

  void Init() {
    outcome = OUTCOME_UNKNOWN;
    order = ORDER_UNKNOWN;
    ownClass = CLASS_UNKNOWN;
    opponentClass = CLASS_UNKNOWN;
  }

  void Update() {
    if(order == ORDER_UNKNOWN) {
      if(FindMarker("going_first")) {
        order = ORDER_FIRST;
      }
      if(FindMarker("going_second")) {
        order = ORDER_SECOND;
      }
    }
    if(outcome == OUTCOME_UNKNOWN) {
      if(FindMarker("victory")) {
        outcome = OUTCOME_VICTORY;
      }
      if(FindMarker("defeat")) {
        outcome = OUTCOME_DEFEAT;
      }
    }
    if(ownClass == CLASS_UNKNOWN || opponentClass == CLASS_UNKNOWN) {
      for(int i = 0; i < NUM_CLASSES; i++) {
        string className = CLASS_NAMES[i];
        if(FindMarker(string("own_class_") + className)) {
          ownClass = (Class)i;
        }
        if(FindMarker(string("opponent_class_") + className)) {
          opponentClass = (Class)i;
        }
      }
    }
  }

  bool Active() {
    return FindMarker("ingame");
  }

  Outcome GetOutcome() const {
    return outcome;
  }

  Class GetOwnClass() const {
    return ownClass;
  }

  Class GetOpponentClass() const {
    return opponentClass;
  }

  GoingOrder GetGoingOrder() const {
    return order;
  }
};
