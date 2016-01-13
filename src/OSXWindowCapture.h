#pragma once

#include "WindowCapture.h"

#include <ApplicationServices/ApplicationServices.h>
#include <QTimer>

class OSXWindowCapture : public QObject, public WindowCapture
{
Q_OBJECT

private:
  QString  mWindowName;
  CGRect   mRect;
  int mWinId;

  static int FindWindow( const QString& name );
  static bool WindowRect( int windowId, CGRect *rect );

  bool Fullscreen();

public:
  OSXWindowCapture( const QString& windowName );

  bool WindowFound();
  int Width();
  int Height();

  QPixmap Capture( int x, int y, int w, int h );
};
