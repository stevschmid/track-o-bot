#include <QApplication.h>
#include <QDir>

#include "hearthstone.h"
#include "dhasher.h"

class Scene
{
protected:
  class Marker {
  public:
    string name;
    dhash hash;
    int x, y, w, h; // virtual canvas attributes!

    Marker():name(""), x(0), y(0), w(0), h(0) {
    }

    Marker(const string& name, const string& sceneFileName, int x, int y, int w, int h)
      :name(name), x(x), y(y), w(w), h(h)
    {
      QPixmap markerPixmap = QPixmap(sceneFileName.c_str()).copy(x, y, w, h);
      markerPixmap.save((name + "_dbg.png").c_str());
      hash = DHasher::Instance()->HashOfPixmap(markerPixmap);
    }
  };

  map<string, Marker> markers;
  string name;

public:
  Scene(const string& name):name(name) {
    Init();
  }

  virtual ~Scene() {
  }

  void AddMarker(const Marker& marker) {
    markers[marker.name] = marker;
  }

  void AddMarker(const string& name, const string& sceneFileName, int x, int y, int w, int h) {
    AddMarker(Marker(name, sceneFileName, x, y, w, h));
  }

  bool FindMarker(const string& name) {
    const Marker& marker = markers[name];
    const QPixmap& capture = Hearthstone::Instance()->Capture(marker.x, marker.y, marker.w, marker.h);

    dhash currentHash = DHasher::Instance()->HashOfPixmap(capture);

    return DHasher::Instance()->Similar(currentHash, marker.hash);
  }

  virtual bool Active() = 0;

  virtual void Init() {
  }

  virtual void Update() {
  }

  const string& GetName() const {
    return name;
  }
};

class MainMenuScene : public Scene
{
public:
  MainMenuScene():Scene("MainMenu") {
    AddMarker("main_menu", "../scenes/main_menu.png", 86, 0, 218, 218);
  }

  bool Active() {
    return FindMarker("main_menu");
  }

  void Init() { }
  void Update() { }
};

typedef enum {
  CONSTRUCTED_MODE_UNKNOWN,
  CONSTRUCTED_MODE_CASUAL,
  CONSTRUCTED_MODE_RANKED
} ConstructedMode;

class ConstructedScene : public Scene
{
public:
  ConstructedMode mode;

public:
  ConstructedScene()
    :Scene("Constructed")
  {
    AddMarker("constructed_casual_selected", "../scenes/constructed_casual.png", 687, 91, 140, 126);
    AddMarker("constructed_ranked_selected", "../scenes/constructed_ranked.png", 928, 66, 20, 115);
  }

  void Init() {
    mode = CONSTRUCTED_MODE_UNKNOWN;
  }

  void Update() {
    if(FindMarker("constructed_casual_selected")) {
      mode = CONSTRUCTED_MODE_CASUAL;
    }
    if(FindMarker("constructed_ranked_selected")) {
      mode = CONSTRUCTED_MODE_RANKED;
    }
  }

  bool Active() {
    return FindMarker("constructed_casual_selected") ||
           FindMarker("constructed_ranked_selected");
  }
};

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

class IngameScene : public Scene
{
protected:
  IngameCoinToss coinToss;
  IngameOutcome outcome;

public:
  IngameScene()
    :Scene("Ingame")
  {
    AddMarker("ingame", "../scenes/ingame_victory_v2.png", 79, 327, 49, 63);
    AddMarker("going_first", "../scenes/ingame_mulligan_1st.png", 751, 354, 55, 55);
    AddMarker("going_second", "../scenes/ingame_mulligan_2nd.png", 746, 355, 55, 55);
    AddMarker("victory", "../scenes/ingame_victory_v2.png", 442, 148, 55, 55);
    AddMarker("defeat", "../scenes/ingame_defeat_v2.png", 444, 547, 55, 55);
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

class SceneManager
{
protected:
  Scene *currentScene;
  vector<Scene*> scenes;

  void RegisterScene(Scene *scene) {
    scenes.push_back(scene);
  }


  Scene *findActiveScene() {
    vector<Scene*>::iterator it;
    for(it = scenes.begin(); it != scenes.end(); it++) {
      Scene *scene = *it;
      if(scene->Active()) {
        return scene;
      }
    }
    return NULL;
  }

public:
  SceneManager()
    :currentScene(NULL)
  {
    RegisterScene(new MainMenuScene);
    RegisterScene(new ConstructedScene);
    RegisterScene(new IngameScene);
  }

  ~SceneManager() {
    vector<Scene*>::iterator it;
    for(it = scenes.begin(); it != scenes.end(); ++it) {
      delete *it;
    }
  }

  void Update() {
    if(currentScene) {
      currentScene->Update();
    }

    Scene *newScene = findActiveScene();
    if(newScene && newScene != currentScene) {
      newScene->Init();
      if(currentScene) {
        Transition(currentScene, newScene);
      }
      currentScene = newScene;
    }
  }

  void Transition(Scene *oldScene, Scene *newScene) {
    cout << "Transition from " << oldScene->GetName() << " to " << newScene->GetName() << endl;
  }

  const Scene* ActiveScene() {
    return currentScene;
  }
};

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QDir::setCurrent(app.applicationDirPath());

  OSXWindowCapture capture("Hearthstone");
  Hearthstone::Instance()->SetWindowCapture(&capture);

  SceneManager sceneManager;

  while(1) {
    sceneManager.Update();
    const Scene *activeScene = sceneManager.ActiveScene();
    if(activeScene) {
      cout << "ActiveScene " << activeScene->GetName() << endl;
    }
    usleep(100000);
  }

  /* Hearthstone hs; */
  /* QPixmap pixmap1 = hs.Capture(470, 281, 50, 20); */
  /* dhash value1 = DHasher::Instance()->HashOfPixmap(pixmap1); */
  /* std::cout << "Value 1 " << value1 << endl; */
  /* sleep(1); */

  /* QPixmap pixmap2 = hs.Capture(470, 281, 50, 20); */
  /* dhash value2 = DHasher::Instance()->HashOfPixmap(pixmap2); */
  /* std::cout << "Value 2 " << value2 << endl; */

  /* std::cout << "Hamming Distance " << DHasher::Instance()->CalculateHammingDistance(value1, value2) << endl; */

  return 0;
}
