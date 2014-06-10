#pragma once

#include "../phash.h"
#include "../hearthstone.h"
#include "../generated_markers.h"

#include <QPixmap>

#define ADD_GENERATED_MARKER(NAME, DEF) AddMarker(NAME, DEF##_PATH, DEF##_X, DEF##_Y, DEF##_WIDTH, DEF##_HEIGHT)

class Scene
{
private:
  class Marker {
  public:
    string name;
    phash hash;
    int x, y, w, h; // virtual canvas attributes!

    Marker():name(""), x(0), y(0), w(0), h(0) {
    }

    Marker(const string& name, const string& templateImagePath, int x, int y, int w, int h)
      :name(name), x(x), y(y), w(w), h(h)
    {
      hash = phash_for_pixmap(QPixmap(templateImagePath.c_str()));
    }
  };

  map<string, Marker> markers;
#ifdef _DEBUG
  map<string, bool> debugMarkerFound;
#endif
  string name;

  int originX, originY;

public:
  Scene(const string& name):name(name), originX(0), originY(0) {
    Reset();
  }

  virtual ~Scene() {
  }

  void AddMarker(const string& name, const string& templateImagePath, int x, int y, int w, int h) {
    markers[name] = Marker(name, templateImagePath, x, y, w, h);
  }

  bool FindMarker(const string& name, int dx = 0, int dy = 0) {
    const Marker& marker = markers[name];
    const QPixmap& capture = Hearthstone::Instance()->Capture(
        originX + marker.x + dx,
        originY + marker.y + dy,
        marker.w,
        marker.h);
    phash currentHash = phash_for_pixmap(capture);
    bool similar = phash_check_similarity(currentHash, marker.hash);
#ifdef _DEBUG
    if(debugMarkerFound[name] != similar) {
      LOG("Marker %s in Scene %s %s", name.c_str(),
          GetName().c_str(),
          similar ? "found" : "disappeared");
      debugMarkerFound[name] = similar;
    }
#endif
    return similar;
  }

  virtual bool Active() = 0;

  virtual void Reset() {
    SetOrigin(0, 0);
  }

  virtual void Update() {
  }

  void SetOrigin(int x, int y) {
    originX = x;
    originY = y;
  }

  int GetOriginX() {
    return originX;
  }

  int GetOriginY() {
    return originY;
  }

  const string& GetName() const {
    return name;
  }
};
