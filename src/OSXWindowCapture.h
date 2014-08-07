#pragma once

#include "WindowCapture.h"

#include <ApplicationServices/ApplicationServices.h>
#include <QTimer>

// FindWindow is quite intensive in terms of performance
#define OSX_UPDATE_WINDOW_DATA_INTERVAL 3000 // ms

class OSXWindowCapture : public QObject, public WindowCapture
{
Q_OBJECT

private:
  QTimer *timer;

  string name;
  int winId;
  CGRect rect;

  static int FindWindow( const string& name );
  static bool GetWindowRect( int windowId, CGRect *rect );

  bool IsFullscreen();

private slots:
  void Update();

public:
  OSXWindowCapture( const string& windowName );

  bool WindowFound();
  int GetWidth();
  int GetHeight();

  QPixmap Capture( int x, int y, int w, int h );
};
