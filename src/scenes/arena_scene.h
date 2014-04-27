class ArenaScene : public Scene
{
public:
  ArenaScene()
    :Scene("Arena")
  {
    AddMarker("arena", ":/scenes/arena.png", 240, 627, 50, 50);
  }

  void Init() {
  }

  void Update() {
  }

  bool Active() {
    return FindMarker("arena");
  }
};
