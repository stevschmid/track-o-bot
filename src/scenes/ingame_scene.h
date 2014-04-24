class IngameScene : public Scene
{
protected:
  Outcome outcome;
  bool coin;
  Class ownClass;
  Class opponentClass;
  GameMode mode;

public:
  IngameScene()
    :Scene("Ingame")
  {
    AddMarker("ingame", "../scenes/ingame_victory_v2.png", 972, 523, 31, 29);
    AddMarker("going_first", "../scenes/ingame_mulligan_1st.png", 751, 354, 55, 55);
    AddMarker("going_second", "../scenes/ingame_mulligan_2nd.png", 746, 355, 55, 55);
    AddMarker("victory", "../scenes/ingame_victory_v2.png", 442, 148, 55, 55);
    AddMarker("defeat", "../scenes/ingame_defeat_v2.png", 444, 547, 55, 55);

    for(int i = 0; i < NUM_CLASSES; i++ ) {
      AddMarker(string("own_class_") + CLASS_NAMES[i],
          string("../scenes/ingame_") + CLASS_NAMES[i] + ".png",
          487, 552, 50, 50);
      AddMarker(string("opponent_class_") + CLASS_NAMES[i],
          string("../scenes/ingame_") + CLASS_NAMES[i] + ".png",
          487, 105, 50, 50);
    }
  }

  void Init() {
    mode = MODE_UNKNOWN;
    outcome = OUTCOME_UNKNOWN;
    coin = false;
    ownClass = CLASS_UNKNOWN;
    opponentClass = CLASS_UNKNOWN;
  }

  void Update() {
    if(FindMarker("going_first")) {
      coin = false;
    }
    if(FindMarker("going_second")) {
      coin = true;
    }
    if(FindMarker("victory")) {
      outcome = OUTCOME_VICTORY;
    }
    if(FindMarker("defeat")) {
      outcome = OUTCOME_DEFEAT;
    }
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

  bool Active() {
    return FindMarker("ingame");
  }

  void SetGameMode(GameMode newMode) {
    mode = newMode;
  }

  GameMode GetGameMode() {
    return mode;
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

  bool GetCoin() const {
    return coin;
  }
};
