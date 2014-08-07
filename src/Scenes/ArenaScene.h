class ArenaScene : public Scene
{
public:
  ArenaScene()
    : Scene( "Arena" )
  {
    ADD_GENERATED_MARKER( "arena", ARENA_ID );
  }

  bool Active() {
    return FindMarker( "arena" );
  }
};
