#include "LinuxWindowCapture.h"

// remove the window title bar which we are not interested in
#define LINUX_WINDOW_TITLE_BAR_HEIGHT 22

LinuxWindowCapture::LinuxWindowCapture( const string& windowName )
  : mWindowName( windowName ), mWinId( 0 )
{
  mTimer = new QTimer( this );
  connect( mTimer, SIGNAL( timeout() ), this, SLOT( Update() ) );
  mTimer->start( LINUX_UPDATE_WINDOW_DATA_INTERVAL );

  Update();
}

void LinuxWindowCapture::Update() {
  if( mWinId == 0 ) {
    mWinId = FindWindow( mWindowName );
  }

  if( mWinId && !WindowRect( mWinId, &mRect ) ) {
    // Window became invalid
    mWinId = 0;
  }
}

bool LinuxWindowCapture::WindowFound() {
  return mWinId != 0;
}

int LinuxWindowCapture::Width() {
  return mRect.width();
}

int LinuxWindowCapture::Height() {
  int height = mRect.height();
  return Fullscreen() ? height : std::max< int >( height - LINUX_WINDOW_TITLE_BAR_HEIGHT, 0 );
}

QPixmap LinuxWindowCapture::Capture( int x, int y, int w, int h ) {
    //LOG("Capturing window: %d, %d, %d, %d", x,y,h,w);
    QPixmap pixmap = QPixmap::grabWindow(mWinId,
                                         x + mRect.x(),
                                         y + mRect.y() + ( Fullscreen() ? 0 : LINUX_WINDOW_TITLE_BAR_HEIGHT ),
                                         w,
                                         h );
  return pixmap;
}

bool LinuxWindowCapture::Fullscreen() {
  // this is not the most elegant solution, but I couldn't find a better way
  return mRect.x() == 0.0f && mRect.y() == 0.0f &&
    ( mRect.height() & LINUX_WINDOW_TITLE_BAR_HEIGHT ) != LINUX_WINDOW_TITLE_BAR_HEIGHT;
}

QList<Window> LinuxWindowCapture::listXWindowsRecursive(Display *disp, Window w)
{
    Window root;
    Window parent;
    Window *children;
    unsigned int childrenCount;

    QList<Window> windows;
    if(XQueryTree(disp, w, &root, &parent, &children, &childrenCount))
    {
        for(unsigned int i = 0; i < childrenCount; ++i)
        {
            windows << children[i];
            windows << listXWindowsRecursive(disp, children[i]);
        }
        XFree(children);
    }
    return windows;
}

int LinuxWindowCapture::FindWindow( const string& name ) {
    int winId = 0;
    Display *disp = XOpenDisplay(":0.0");
    Window rootWin = XDefaultRootWindow(disp);
    QList<Window> windows = listXWindowsRecursive(disp, rootWin);

    foreach(Window win, windows){
        char *n;
        XFetchName(disp, win, &n);
        QString needle = QString::fromStdString(name);
        QString found = QString::fromLocal8Bit(n);
        if(found == needle){
            winId = win;
        }
    }
    XCloseDisplay(disp);
  return winId;
}

bool LinuxWindowCapture::WindowRect( int windowId, QRect *rect ) {
    Display *disp = XOpenDisplay(":0.0");
    QList<Window> windows = listXWindowsRecursive(disp, windowId);

    int numWindows = windows.length();
    if(numWindows > 0){
        int x,y;
        unsigned int h,w,border,depth;
        Window root;
        XGetGeometry(disp, windows.at(0), &root, &x, &y, &h, &w, &border, &depth);
        rect->setRect(x, y, w, h);
        //LOG("Windows geometry: %d, %d, %d, %d", x,y,h,w);
    }

    XCloseDisplay(disp);
  return numWindows > 0;
}
