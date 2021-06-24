#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h> // memset
#include "debug.h"


typedef struct {
  unsigned int id;
  unsigned int delayms;
} timer_t;


typedef struct {
  int rx;
  int ry;
  bool keys[ 256 ];
  timer_t render_timer;
} window_t;
// wndproc arguments list is predefined so any optional value
// passed to the winproc must be scoped globally.
window_t w;


int
init_w () {
  w.rx = 1;
  w.ry = 1;
  w.render_timer.id = 1;
  w.render_timer.delayms = 100;
  memset ( w.keys, false, 256 * sizeof *w.keys );
}


int
paint_rect ( HDC hdc, int x, int y, int w, int h, COLORREF color ) {
  HBRUSH b = CreateSolidBrush ( color );
  const RECT r = { x, y, x + w, y + h };
  FillRect ( hdc, &r, b );
  DeleteObject ( b );
}


LRESULT CALLBACK 
wndproc ( HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam ) { 

  if ( umsg == WM_DESTROY ) {
    PostQuitMessage ( 0 );
    return 0;
  }
  
  else if ( umsg == WM_PAINT ) {
    PAINTSTRUCT ps = {};
    HDC hdc = BeginPaint ( hwnd, &ps );
    const int width = ps.rcPaint.right - ps.rcPaint.left;
    const int height = ps.rcPaint.bottom - ps.rcPaint.top;
    HDC cdc = CreateCompatibleDC ( hdc );
    HBITMAP hbmp = CreateCompatibleBitmap ( hdc, width, height );
    HBITMAP oldhbmp = ( HBITMAP ) SelectObject ( cdc, hbmp );
    FillRect ( cdc, &ps.rcPaint, GetSysColorBrush ( COLOR_BACKGROUND ) );
    
    paint_rect ( cdc, w.rx, w.ry, 40, 40, RGB ( 255, 0, 0 ) );
    w.rx += 2;
    w.ry += 1;
    
    BitBlt ( hdc, 0, 0, width, height, cdc, 0, 0, SRCCOPY );
    SelectObject ( cdc, oldhbmp );
    DeleteObject ( hbmp );
    DeleteDC ( cdc );
    EndPaint ( hwnd, &ps );
    return 0;
  }
  
  else if ( umsg == WM_KEYDOWN ) { 
    w.keys[ ( uint8_t ) wparam ] = true;
    if ( wparam == VK_ESCAPE ) { // exit on escape
      PostQuitMessage ( 0 );
    }
    return 0;
  }

  else if ( umsg == WM_KEYUP ) {
    w.keys[ ( uint8_t ) wparam ] = false;    
    return 0;
  }

  else if ( umsg == WM_TIMER ) {
    if ( wparam == w.render_timer.id ) { // periodic paint msg
      InvalidateRect ( hwnd, NULL, FALSE );
    }
    return 0;
  }
  
  else if ( umsg == WM_SETFOCUS ) {
    SetTimer ( hwnd, w.render_timer.id, w.render_timer.delayms, NULL );
    return 0;
  }
  
  else if ( umsg == WM_KILLFOCUS ) {
    KillTimer ( hwnd, w.render_timer.id );
    return 0;
  }
  
  return DefWindowProc ( hwnd, umsg, wparam, lparam );
  
}


int
init_hwnd
  ( HWND *hwnd
  , const wchar_t *title
  , const int x
  , const int y
  , const int w
  , const int h
  , HINSTANCE hInstance
  )
{
  
  WNDCLASS wc = {};
  wc.lpfnWndProc   = wndproc;
  wc.hInstance     = hInstance;
  wc.lpszClassName = title;
  RegisterClass ( &wc );

  *hwnd = CreateWindowEx
    ( 0                    // Optional window styles
    , wc.lpszClassName     // Window class
    , title                // Window text
    , WS_OVERLAPPEDWINDOW  // Window style
    , x
    , y
    , w
    , h
    , NULL                 // Parent window
    , NULL                 // Menu
    , hInstance            // Instance handle
    , NULL                 // Additional application data      
    );

  if ( *hwnd == NULL ) ERR ( ERR_HWND_IS_NULL );
  
  return 0;
  
}

