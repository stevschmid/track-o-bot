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

    Marker(const string& name, const string& sceneFileName, int x, int y, int w, int h)
      :name(name), x(x), y(y), w(w), h(h)
    {
      QPixmap markerPixmap = QPixmap(sceneFileName.c_str()).copy(x, y, w, h);
      markerPixmap.save((name + "_dbg.png").c_str());
      hash = DHasher::Instance()->HashOfPixmap(markerPixmap);
    }
  };

  vector<Marker> markers;

public:

  void AddMarker(const Marker& marker) {
    markers.push_back(marker);
  }

  void AddMarker(const string& name, const string& sceneFileName, int x, int y, int w, int h) {
    AddMarker(Marker(name, sceneFileName, x, y, w, h));
  }

  void CheckMarkers() {
    vector<Marker>::iterator it;
    for(it = markers.begin(); it != markers.end(); ++it) {
      const Marker& marker = *it;
      const QPixmap& capture = Hearthstone::Instance()->Capture(marker.x, marker.y, marker.w, marker.h);

      dhash currentHash = DHasher::Instance()->HashOfPixmap(capture);

      if(DHasher::Instance()->Similar(currentHash, marker.hash)) {
        cout << marker.name << endl;
      }
    }
  }

};

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QDir::setCurrent(app.applicationDirPath());

  capture = OSXWindowCapture("Hearthstone");
  Hearthstone::Instance()->SetWindowCapture(&capture);

  Scene scene;

  scene.AddMarker("main_menu_key", "../scenes/main_menu.png", 205, 118, 100, 100);
  scene.AddMarker("constructed_key", "../scenes/constructed_casual.png", 443, 75, 70, 70);
  scene.AddMarker("constructed_casual_selected", "../scenes/constructed_casual.png", 669, 37, 60, 60);
  scene.AddMarker("constructed_ranked_selected", "../scenes/constructed_ranked.png", 927, 25, 75, 75);

  scene.AddMarker("main_menu_key", "../scenes/main_menu.png", 205, 118, 100, 100);
  scene.AddMarker("constructed_key", "../scenes/constructed_casual.png", 443, 75, 70, 70);
  scene.AddMarker("constructed_casual_selected", "../scenes/constructed_casual.png", 669, 37, 60, 60);
  scene.AddMarker("constructed_ranked_selected", "../scenes/constructed_ranked.png", 927, 25, 75, 75);

  scene.AddMarker("mulligan_key", "../scenes/ingame_mulligan_1st.png", 613, 691, 150, 50);
  scene.AddMarker("going_first", "../scenes/ingame_mulligan_1st.png", 751, 354, 55, 55);
  scene.AddMarker("going_second", "../scenes/ingame_mulligan_2nd.png", 746, 355, 55, 55);

  scene.AddMarker("victory", "../scenes/ingame_victory_v2.png", 442, 148, 55, 55);
  scene.AddMarker("defeat", "../scenes/ingame_defeat_v2.png", 444, 547, 55, 55);

  while(1) {
    scene.CheckMarkers();
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
