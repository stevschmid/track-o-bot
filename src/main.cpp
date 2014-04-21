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
      capture.save("debug_capture.png");

      dhash currentHash = DHasher::Instance()->HashOfPixmap(capture);

      cout << "Marker " << marker.name << " differs by " << DHasher::Instance()->CalculateHammingDistance(currentHash, marker.hash) << " bits " << endl;
      cout << "Similar " << DHasher::Instance()->Similar(currentHash, marker.hash) << endl;
    }
  }

};

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  cout << "yolo " << app.applicationDirPath().toStdString() << endl;
  QDir::setCurrent(app.applicationDirPath());

  Scene mainMenu;
  mainMenu.AddMarker("key1", "../scenes/main_menu.png", 205, 118, 100, 100);

  while(1) {
    mainMenu.CheckMarkers();
    sleep(1);
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
