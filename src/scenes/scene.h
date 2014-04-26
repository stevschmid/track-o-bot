#pragma once

#include "../dhash.h"
#include "../hearthstone.h"

#include <QPixmap>

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
      /* markerPixmap.save((name + "_dbg.png").c_str()); */
      hash = dhash_for_pixmap(markerPixmap);
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
    dhash currentHash = dhash_for_pixmap(capture);
    return dhash_check_similarity(currentHash, marker.hash);
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
