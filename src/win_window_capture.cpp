#include "win_window_capture.h"

WinWindowCapture::WinWindowCapture(const string& windowName)
  :name(windowName)
{
}

HWND WinWindowCapture::FindHWND() {
  return FindWindowA(NULL, "Hearthstone");
}

RECT WinWindowCapture::GetRect() {
  RECT rect = { 0 };
  if(HWND hwnd = FindHWND()) {
    GetClientRect(hwnd, &rect);
  }
  return rect;
}

bool WinWindowCapture::WindowFound() {
  return FindHWND() != NULL;
}

int WinWindowCapture::GetWidth() {
  RECT rect = GetRect();
  return rect.right - rect.left;
}

int WinWindowCapture::GetHeight() {
  RECT rect = GetRect();
  return rect.bottom - rect.top;
}

QPixmap WinWindowCapture::Capture(int x, int y, int w, int h) {
  HWND hwnd = FindHWND();
  QPixmap pixmap;

  if(hwnd) {
    HDC hDCWindow = GetDC(hwnd);
    HDC hDCMem = CreateCompatibleDC(hDCWindow);

    HBITMAP hBitmap = CreateCompatibleBitmap(hDCWindow, w, h);
    SelectObject(hDCMem, hBitmap);
    BitBlt(hDCMem, 0, 0, w, h, hDCWindow, x, y, SRCCOPY);

    pixmap = QPixmap::fromWinHBITMAP(hBitmap);

    ReleaseDC(hwnd, hDCWindow);
    DeleteDC(hDCMem);
    DeleteObject(hBitmap);
  }

  return pixmap;
}

