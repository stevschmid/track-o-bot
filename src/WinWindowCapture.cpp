#include "WinWindowCapture.h"

#include <Windows.h>
#include <QtWinExtras/qwinfunctions.h>

WinWindowCapture::WinWindowCapture( const QString& windowName )
  : mWindowName( windowName )
{
}

HWND WinWindowCapture::FindHWND() {
  return FindWindowW( NULL, mWindowName.toStdWString().c_str() );
}

RECT WinWindowCapture::Rect() {
  RECT rect = { 0 };
  if( HWND hwnd = FindHWND() ) {
    GetClientRect( hwnd, &rect );
  }
  return rect;
}

POINT WinWindowCapture::ScreenPos() {
  POINT point = { 0 };
  if( HWND hwnd = FindHWND() ) {
    ClientToScreen( hwnd, &point );
  }
  return point;
}

bool WinWindowCapture::WindowFound() {
  return FindHWND() != NULL;
}

int WinWindowCapture::Width() {
  RECT rect = Rect();
  return rect.right - rect.left;
}

int WinWindowCapture::Height() {
  RECT rect = Rect();
  return rect.bottom - rect.top;
}

int WinWindowCapture::Left() {
  return ScreenPos().x + Rect().left;
}

int WinWindowCapture::Top() {
  return ScreenPos().y + Rect().top;
}

QPixmap WinWindowCapture::Capture( int x, int y, int w, int h ) {
  HWND hwnd = FindHWND();
  QPixmap pixmap;

  if( hwnd ) {
    HDC hDCWindow = GetDC( hwnd );
    HDC hDCMem = CreateCompatibleDC( hDCWindow );

    HBITMAP hBitmap = CreateCompatibleBitmap( hDCWindow, w, h );
    SelectObject( hDCMem, hBitmap );
    BitBlt( hDCMem, 0, 0, w, h, hDCWindow, x, y, SRCCOPY );

    pixmap = QtWin::fromHBITMAP( hBitmap );

    ReleaseDC( hwnd, hDCWindow );
    DeleteDC( hDCMem );
    DeleteObject( hBitmap );
  }

  return pixmap;
}

