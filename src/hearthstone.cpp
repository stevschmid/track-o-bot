#include "hearthstone.h"

#ifdef Q_WS_MAC
#include "osx_window_capture.h"
#elif defined Q_WS_WIN
#include "win_window_capture.h"
#endif

DEFINE_SINGLETON_SCOPE(Hearthstone)

Hearthstone::Hearthstone() {
#ifdef Q_WS_MAC
  capture = new OSXWindowCapture("Hearthstone");
#elif defined Q_WS_WIN
  capture = new WinWindowCapture("Hearthstone");
#endif
}

Hearthstone::~Hearthstone() {
  if(capture != NULL)
    delete capture;
}

bool Hearthstone::IsRunning() {
  return capture->WindowFound();
}

#ifdef Q_WS_WIN
inline float roundf(float x) {
   return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif

QPixmap Hearthstone::Capture(int vx, int vy, int vw, int vh) {
  int x, y, w, h;

  int realCanvasWidth = capture->GetWidth();
  int realCanvasHeight = capture->GetHeight();

  int virtualCanvasWidth = VIRTUAL_CANVAS_WIDTH;
  int virtualCanvasHeight = VIRTUAL_CANVAS_HEIGHT;

  float dx = (vx - virtualCanvasWidth/2);
  float dy = (vy - virtualCanvasHeight/2);
  float scale = (float)realCanvasHeight / virtualCanvasHeight;

  // Rounding here is important for dhash calc
  x = roundf(realCanvasWidth/2 + dx * scale);
  y = roundf(realCanvasHeight/2 + dy * scale);
  w = roundf(vw * scale);
  h = roundf(vh * scale);

  return capture->Capture(x, y, w, h);
}

void Hearthstone::SetWindowCapture(WindowCapture *wc) {
  if(capture != NULL)
    delete capture;

  capture = wc;
}
