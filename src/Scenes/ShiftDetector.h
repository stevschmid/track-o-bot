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
  QElapsedTimer mTimer;
  Scene*        mScene;
  string        mMarkerName;
  unsigned int  mGracePeriod;

  bool CheckForSceneShift() {
    bool shiftFound = false;

    const int min = -DETECT_SHIFT_PIXEL_RADIUS;
    const int max = DETECT_SHIFT_PIXEL_RADIUS;

    int dx = min + ( rand() % ( max - min + 1 ) );
    int dy = min + ( rand() % ( max - min + 1 ) );

    if( mScene->FindMarker( mMarkerName, dx, dy ) ) {
      shiftFound = true;

      LOG( "Detected Scene Shift in x:%d y:%d", dx, dy );
      mScene->SetOrigin( mScene->OriginX() + dx, mScene->OriginY() + dy );
    }

    return shiftFound;
  }

public:

  ShiftDetector( Scene *scene, const string& markerName, unsigned int gracePeriod )
    : mScene( scene ), mMarkerName( markerName ), mGracePeriod( gracePeriod )
  {
    Reset();
  }

  void Reset() {
    mTimer.invalidate();
  }

  void Update() {
    if( !mScene->FindMarker( mMarkerName ) ) {
      if( !mTimer.isValid() ) {
        mTimer.start();
      } else {
        if( mTimer.hasExpired( mGracePeriod ) ) {
          CheckForSceneShift();
        }
      }
    } else {
      if( mTimer.isValid() ) {
        mTimer.invalidate();
      }
    }
  }
};
