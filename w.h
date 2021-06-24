#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> // calloc
#include <string.h> // memset, memcpy
#include "debug.h"


typedef struct {
  unsigned int id;
  unsigned int delayms;
} timer_t;


typedef struct {
  size_t width;
  size_t height;
  size_t length;
  uint8_t *pixels;
} display_t;


typedef struct {

  bool keys[ 256 ];
  
  timer_t render_timer;
  
  int time;
  
  display_t d;
  
  void *hbmpraw;
  HBITMAP hbmp;
  
} window_t;
// wndproc arguments list is predefined so any optional value
// passed to the winproc must be scoped globally.
window_t w;


int
kill_w () {
  
  free ( w.d.pixels ); w.d.pixels = NULL;
  w.hbmpraw = NULL;
  DeleteObject ( w.hbmp );
  
}


int
init_w ( HWND hwnd ) {
  
  w.render_timer.id = 1;
  w.render_timer.delayms = 1000 / 60; // 60 frames per second
  
  w.time = GetTickCount ();
  
  memset ( w.keys, false, 256 * sizeof *w.keys );
  
  w.d.width = 128;
  w.d.height = 128;
  w.d.length = w.d.width * w.d.height * 3;
  w.d.pixels = calloc ( w.d.length, sizeof *w.d.pixels );
  
  BITMAPINFOHEADER bmpih =
    { sizeof ( BITMAPINFOHEADER ) // header size
    , w.d.width
    , -w.d.height // negative for top-down DIB
    , 1           // planes, must be 1
    , 24          // 3 bytes per pixel, bmiColors can be NULL
    , BI_RGB      // w\o compression
    , 0           // image size, can be 0 for BI_RGB
    , 0           // unused by CreateDIBSection
    , 0           // unused by CreateDIBSection
    , 0           // used colors, must be 0 for packed bmps
    , 0           // required colors, can be 0
    };
  BITMAPINFO bmpi = { bmpih, ( RGBQUAD ) {} };
  HDC hdc = GetDC ( hwnd );
  w.hbmpraw = NULL;
  w.hbmp = CreateDIBSection ( hdc, &bmpi, DIB_RGB_COLORS, &w.hbmpraw, NULL, 0 );
  
}


LRESULT CALLBACK 
wndproc ( HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam ) { 

  if ( umsg == WM_DESTROY ) {
    PostQuitMessage ( 0 );
    return 0;
  }
  
  else if ( umsg == WM_PAINT ) {
    const int curtime = GetTickCount ();
    const int elapsed = curtime - w.time;
    w.time = curtime;
    PAINTSTRUCT ps = {};
    HDC hdc = BeginPaint ( hwnd, &ps );
    const int width = ps.rcPaint.right - ps.rcPaint.left;
    const int height = ps.rcPaint.bottom - ps.rcPaint.top;
    const int dwidth = w.d.width;
    const int dheight = w.d.height;
    
    uint8_t *pixels = w.d.pixels;
    for ( int y = 0; y < dheight; ++y )
      for ( int x = 0; x < dwidth; ++x )
    {
      pixels[ ( y * dwidth + x ) * 3 + 0 ] = ( uint8_t ) ( x + y + w.time / 10 );
      pixels[ ( y * dwidth + x ) * 3 + 1 ] = ( uint8_t ) ( x * y + w.time / 10 );
      pixels[ ( y * dwidth + x ) * 3 + 2 ] = ( uint8_t ) ( x ^ y + w.time / 10 );
    }
    memcpy ( w.hbmpraw, pixels, w.d.length * sizeof *pixels );
    
    HDC cdc = CreateCompatibleDC ( hdc );
    HBITMAP oldhbmp = ( HBITMAP ) SelectObject ( cdc, w.hbmp );
    StretchBlt ( hdc, 0, 0, width, height, cdc, 0, 0, dwidth, dheight, SRCCOPY );
    SelectObject ( cdc, oldhbmp );
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

