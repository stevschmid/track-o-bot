#include "WinWindowCapture.h"

WinWindowCapture::WinWindowCapture( const string& windowName )
  : mWindowName( windowName )
{
}

HWND WinWindowCapture::FindHWND() {
  return FindWindowA( NULL, mWindowName.c_str() );
}

RECT WinWindowCapture::Rect() {
  RECT rect = { 0 };
  if( HWND hwnd = FindHWND() ) {
    GetClientRect( hwnd, &rect );
  }
  return rect;
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

QPixmap WinWindowCapture::Capture( int x, int y, int w, int h ) {
  HWND hwnd = FindHWND();
  QPixmap pixmap;

  if( hwnd ) {
    HDC hDCWindow = GetDC( hwnd );
    HDC hDCMem = CreateCompatibleDC( hDCWindow );

    HBITMAP hBitmap = CreateCompatibleBitmap( hDCWindow, w, h );
    SelectObject( hDCMem, hBitmap );
    BitBlt( hDCMem, 0, 0, w, h, hDCWindow, x, y, SRCCOPY );

    pixmap = QPixmap::fromWinHBITMAP( hBitmap );

    ReleaseDC( hwnd, hDCWindow );
    DeleteDC( hDCMem );
    DeleteObject( hBitmap );
  }

  return pixmap;
}

