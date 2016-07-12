#pragma once

#include "WindowCapture.h"

#include <ApplicationServices/ApplicationServices.h>
#include <QTimer>

class OSXWindowCapture : public QObject, public WindowCapture
{
Q_OBJECT

private:
  CGRect   mRect;
  int mWinId;
  bool mFocus;

  static int FindWindow( const QString& name );
  static bool ExtractWindowProperties( int windowId, CGRect *rect, bool *focus );

  bool Fullscreen();

public:
  OSXWindowCapture();

  bool WindowFound();
  int Width();
  int Height();

  int Left();
  int Top();

  QPixmap Capture( int x, int y, int w, int h );

  bool HasFocus();
};
