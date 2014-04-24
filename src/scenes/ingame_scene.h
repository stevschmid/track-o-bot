typedef enum {
  INGAME_COIN_TOSS_UNKNOWN,
  INGAME_COIN_TOSS_FIRST,
  INGAME_COIN_TOSS_SECOND
} IngameCoinToss;

typedef enum {
  INGAME_OUTCOME_UNKNOWN,
  INGAME_OUTCOME_VICTORY,
  INGAME_OUTCOME_DEFEAT
} IngameOutcome;

#define NUM_CLASSES 9
const char CLASSES[NUM_CLASSES][128] = {
  "priest",
  "druid",
  "shaman",
  "warrior",
  "warlock",
  "paladin",
  "rogue",
  "hunter",
  "mage"
};

class IngameScene : public Scene
{
protected:
  IngameCoinToss coinToss;
  IngameOutcome outcome;

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
      string className = CLASSES[i];
      AddMarker(string("own_class_") + className, string("../scenes/ingame_") + className + ".png", 487, 552, 50, 50);
      AddMarker(string("opponent_class_") + className, string("../scenes/ingame_") + className + ".png", 487, 105, 50, 50);
    }
  }

  void Init() {
    coinToss = INGAME_COIN_TOSS_UNKNOWN;
    outcome = INGAME_OUTCOME_UNKNOWN;
  }

  void Update() {
    if(FindMarker("going_first")) {
      coinToss = INGAME_COIN_TOSS_FIRST;
    }
    if(FindMarker("going_second")) {
      coinToss = INGAME_COIN_TOSS_SECOND;
    }
    if(FindMarker("victory")) {
      outcome = INGAME_OUTCOME_VICTORY;
    }
    if(FindMarker("defeat")) {
      outcome = INGAME_OUTCOME_DEFEAT;
    }
  }

  bool Active() {
    return FindMarker("ingame");
  }
};
