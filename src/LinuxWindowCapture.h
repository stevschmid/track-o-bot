#pragma once

#include "WindowCapture.h"

#include <X11/Xlib.h>
#include <QTimer>

// FindWindow is quite intensive in terms of performance
#define LINUX_UPDATE_WINDOW_DATA_INTERVAL 3000 // ms

class LinuxWindowCapture : public QObject, public WindowCapture
{
Q_OBJECT

private:
  QTimer*  mTimer;
  string   mWindowName;
  int      mWinId;
  QRect   mRect;

  static int FindWindow( const string& name );
  static bool WindowRect( int windowId, QRect *rect );
  static QList<Window> listXWindowsRecursive(Display *disp, Window w);

  bool Fullscreen();

private slots:
  void Update();

public:
  LinuxWindowCapture( const string& windowName );

  bool WindowFound();
  int Width();
  int Height();

  QPixmap Capture( int x, int y, int w, int h );
};
