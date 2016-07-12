#include "WinWindowCapture.h"

#include <Windows.h>
#include <QtWinExtras/qwinfunctions.h>
#include <psapi.h>

WinWindowCapture::WinWindowCapture()
  : mHwnd( NULL )
{
}

HWND WinWindowCapture::FindHWND() {
  if( mHwnd && IsWindow( mHwnd ) ) {
    return mHwnd;
  }

  HWND hwnd = FindWindowW( NULL, L"Hearthstone" );

  if( !hwnd ) {
    // Fallback for localized
    HWND unityHwnd = FindWindowW( L"UnityWndClass", NULL );
    if( unityHwnd ) {
      DWORD procId;
      GetWindowThreadProcessId( unityHwnd, &procId );

      HANDLE handle = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, procId );
      TCHAR buffer[ MAX_PATH ] = { 0 };
      GetModuleFileNameEx( (HMODULE)handle, NULL, buffer, MAX_PATH );
      CloseHandle( handle );

      QString path = QString::fromWCharArray( buffer );
      if( path.contains( "Hearthstone" ) ) {
        hwnd = unityHwnd;
      }
    }
  }

  mHwnd = hwnd;
  return mHwnd;
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

bool WinWindowCapture::HasFocus() {
  return GetForegroundWindow() == FindHWND();
}

