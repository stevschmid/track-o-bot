#pragma once

#include "Scene.h"

#include <QElapsedTimer>

/*
 * This is a class for a rather particular case
 * Heavy minions and some spells cause the ingame board to shake
 * This shaking can cause the screen origin to shift permanently,
 * i.e. the whole ingame screen is offset by a few pixel after the shaking
 * The image fingerprint can only deal with a small offset.
 *
 * This class detects shifts by probing for a marker and shifts the
 * screen origin after after the marker has been found (by random walking every frame)
 */

#define DETECT_SHIFT_PIXEL_RADIUS 15

class ShiftDetector
{
private:

  QElapsedTimer timer;
  Scene *scene;
  string markerName;
  unsigned int gracePeriod;

  bool CheckForSceneShift() {
    bool shiftFound = false;

    const int min = -DETECT_SHIFT_PIXEL_RADIUS;
    const int max = DETECT_SHIFT_PIXEL_RADIUS;

    int dx = min + (rand() % (max - min + 1));
    int dy = min + (rand() % (max - min + 1));

    if(scene->FindMarker(markerName, dx, dy)) {
      shiftFound = true;

      LOG("Detected Scene Shift in x:%d y:%d", dx, dy);
      scene->SetOrigin(scene->GetOriginX() + dx,
                       scene->GetOriginY() + dy);
    }

    return shiftFound;
  }

public:

  ShiftDetector(Scene *scene, const string& markerName, unsigned int gracePeriod)
    :scene(scene), markerName(markerName), gracePeriod(gracePeriod)
  {
    Reset();
  }

  void Reset() {
    timer.invalidate();
  }

  void Update() {
    if(!scene->FindMarker(markerName)) {
      if(!timer.isValid()) {
        timer.start();
      } else {
        if(timer.hasExpired(gracePeriod)) {
          CheckForSceneShift();
        }
      }
    } else {
      if(timer.isValid()) {
        timer.invalidate();
      }
    }
  }
};
