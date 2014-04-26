class IngameScene : public Scene
{
protected:
  Outcome outcome;
  GoingOrder order;
  Class ownClass;
  Class opponentClass;

public:
  IngameScene()
    :Scene("Ingame")
  {
    AddMarker("ingame", ":/scenes/ingame_victory_v2.png", 972, 523, 31, 29);
    AddMarker("going_first", ":/scenes/ingame_mulligan_1st.png", 751, 354, 55, 55);
    AddMarker("going_second", ":/scenes/ingame_mulligan_2nd.png", 746, 355, 55, 55);
    AddMarker("victory", ":/scenes/ingame_victory_v2.png", 442, 148, 55, 55);
    AddMarker("defeat", ":/scenes/ingame_defeat_v2.png", 444, 547, 55, 55);

    for(int i = 0; i < NUM_CLASSES; i++ ) {
      AddMarker(string("own_class_") + CLASS_NAMES[i],
          string(":/scenes/ingame_") + CLASS_NAMES[i] + ".png",
          487, 552, 50, 50);
      AddMarker(string("opponent_class_") + CLASS_NAMES[i],
          string(":/scenes/ingame_") + CLASS_NAMES[i] + ".png",
          487, 105, 50, 50);
    }
  }

  void Init() {
    outcome = OUTCOME_UNKNOWN;
    order = GOING_UNKNOWN;
    ownClass = CLASS_UNKNOWN;
    opponentClass = CLASS_UNKNOWN;
  }

  void Update() {
    if(order == GOING_UNKNOWN) {
      if(FindMarker("going_first")) {
        order = GOING_FIRST;
      }
      if(FindMarker("going_second")) {
        order = GOING_SECOND;
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
          logger << " Set Opponent Class to " << opponentClass << endl;
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
