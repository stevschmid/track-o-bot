#include "hearthstone.h"

#include "osx_window_capture.h"

DEFINE_SINGLETON_SCOPE(Hearthstone)

Hearthstone::Hearthstone() {
  capture = new OSXWindowCapture("Hearthstone");
}

Hearthstone::~Hearthstone() {
  if(capture != NULL)
    delete capture;
}

bool Hearthstone::IsRunning() {
  return capture->WindowFound();
}

QPixmap Hearthstone::Capture(int vx, int vy, int vw, int vh) {
  int x, y, w, h;

  int realCanvasWidth = capture->GetWidth();
  int realCanvasHeight = capture->GetHeight();

  int virtualCanvasWidth = VIRTUAL_CANVAS_WIDTH;
  int virtualCanvasHeight = VIRTUAL_CANVAS_HEIGHT;

  float scaleRatioX = (float)realCanvasWidth / virtualCanvasWidth;
  float scaleRatioY = (float)realCanvasHeight / virtualCanvasHeight;

  float dx = (vx - virtualCanvasWidth/2);
  float dy = (vy - virtualCanvasHeight/2);

  float realCanvasAspectRatio = (float)realCanvasWidth / realCanvasHeight;
  float virtualCanvasAspectRatio = (float)virtualCanvasWidth / virtualCanvasHeight;

  w = vw * scaleRatioX;
  h = vh * scaleRatioY;

  float canvasRatio = virtualCanvasAspectRatio / realCanvasAspectRatio;

  if(canvasRatio < 1.0f) {
    // width relative to height is increased compared to the virtual canvas, meaning
    // the image is squeezed in x
    dx *= canvasRatio;
    w *= canvasRatio;
  } else if(canvasRatio > 1.0f) {
    // same but height is squeezed in y
    dy *= canvasRatio;
    h *= canvasRatio;
  }

  x = realCanvasWidth/2 + dx * scaleRatioX;
  y = realCanvasHeight/2 + dy * scaleRatioY;

  QPixmap realmap = capture->Capture(x, y, w, h);
  QPixmap virtualmap = realmap.scaled(vw, vh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  return virtualmap;
}

void Hearthstone::SetWindowCapture(WindowCapture *wc) {
  if(capture != NULL)
    delete capture;

  capture = wc;
}
