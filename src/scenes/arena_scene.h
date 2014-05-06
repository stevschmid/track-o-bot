class ArenaScene : public Scene
{
public:
  ArenaScene()
    :Scene("Arena")
  {
    ADD_GENERATED_MARKER("arena", ARENA_ID);
  }

  void Init() {
  }

  void Update() {
  }

  bool Active() {
    return FindMarker("arena");
  }
};
