#ifndef W_H
#define W_H
#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> // calloc
#include <string.h> // memset, memcpy
#include <time.h> // time
#include "err.h"
// #include "sprog.h"
#include "map.h"
#include "bmpi.h"
#include "imgd.h"


typedef struct {
  size_t x;
  size_t y;
} mouse_t;


typedef struct {
  size_t id;
  size_t delayms;
} timer_t;


typedef struct {
  size_t width;
  size_t height;
  size_t length;
  uint8_t *pixels;
} display_t;


typedef struct {

  mouse_t m;

  bool keys[ 256 ];

  timer_t render_timer;

  size_t time;
  size_t time0;

  display_t d;

  void *hbmpraw;
  HBITMAP hbmp;

  size_t width;
  size_t height;

  map_t map;

  bmpi_t *bmpis;
  size_t bmpis_length;

  texture_t *textures;
  size_t textures_length;

} window_t;
// wndproc arguments list is predefined so any optional value
// passed to the winproc must be scoped globally.
window_t w;


int
kill_w () {

  free ( w.d.pixels ); w.d.pixels = NULL;
  DeleteObject ( w.hbmp );
  w.hbmpraw = NULL;
  map_kill ( &w.map );

  for ( size_t i = 0; i < w.bmpis_length; ++i ) {
    bmpi_kill ( &w.bmpis[ i ] );
  }
  w.bmpis = NULL;

}


int
init_w ( HWND hwnd, size_t dw, size_t dh ) {

  srand ( time( NULL ) );

  w.m.x = 0; // will be init on WM_MOUSEMOVE
  w.m.y = 0;

  w.render_timer.id = 1;
  w.render_timer.delayms = 1000 / 60; // 60 frames per second

  w.time = GetTickCount ();
  w.time0 = 0;

  memset ( w.keys, false, 256 * sizeof *w.keys );

  w.d.width = dw;
  w.d.height = dh;
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
  BITMAPINFO bmpi = { bmpih, { [ 0 ] = ( RGBQUAD ) {} } };
  HDC hdc = GetDC ( hwnd );
  w.hbmpraw = NULL;
  w.hbmp = CreateDIBSection ( hdc, &bmpi, DIB_RGB_COLORS, &w.hbmpraw, NULL, 0 );

  w.width  = 0; // will be init on WM_SIZE after window appear
  w.height = 0;

  map_init ( &w.map, 33, 33 );
  map_quadrantify ( &w.map );
  map_crossify ( &w.map );
  map_borderify ( &w.map );

  imgdecode ( &w.bmpis, &w.bmpis_length, &w.textures, &w.textures_length );

}


int
paint_w ( HWND hwnd ) {

  const size_t time = GetTickCount ();
  const size_t elapsed = time - w.time;
  w.time0 += elapsed;
  w.time = time;
  PAINTSTRUCT ps = {};
  HDC hdc = BeginPaint ( hwnd, &ps );
  const int top = ps.rcPaint.top;
  const int left = ps.rcPaint.left;
  const int width = ps.rcPaint.right - ps.rcPaint.left;
  const int height = ps.rcPaint.bottom - ps.rcPaint.top;
  const int dwidth = w.d.width;
  const int dheight = w.d.height;

  uint8_t *pixels = w.d.pixels;

  memset ( pixels, 0, w.d.length * sizeof *pixels );

  // static int colors[ 16 ];
  // static bool colors_gen = true;
  // if ( colors_gen ) {
    // colors_gen = false;
    // for ( int i = 0; i < 16; ++i ) {
      // colors[ i ] = rand () % 0x1000000;
    // }
  // }

  const size_t textures_index = ( ( size_t ) ( w.time0 / 2000 ) ) % w.textures_length;
  const size_t t_index = ( ( size_t ) ( w.time0 / 500 ) ) % w.textures[ textures_index ].length;


  for ( size_t y = 0; y < dheight; ++y )
    for ( size_t x = 0; x < dwidth; ++x )
  {



    bmpi_rgb24_t *rgb24 =
      bmpi_rgb24_at
        ( w.textures[ textures_index ].bmpis[ t_index ]
        , x % w.textures[ textures_index ].bmpis[ t_index ]->w
        , y % w.textures[ textures_index ].bmpis[ t_index ]->h
        );

    if ( rgb24 == NULL ) ERR ( -100 );

    pixels[ ( y * dwidth + x ) * 3 + 2 ] = ( uint8_t ) rgb24->r;
    pixels[ ( y * dwidth + x ) * 3 + 1 ] = ( uint8_t ) rgb24->g;
    pixels[ ( y * dwidth + x ) * 3 + 0 ] = ( uint8_t ) rgb24->b;
  }



  memcpy ( w.hbmpraw, pixels, w.d.length * sizeof *pixels );

  HDC cdc = CreateCompatibleDC ( hdc );
  HBITMAP oldhbmp = ( HBITMAP ) SelectObject ( cdc, w.hbmp );
  StretchBlt ( hdc, left, top, width, height, cdc, 0, 0, dwidth, dheight, SRCCOPY );
  SelectObject ( cdc, oldhbmp );
  DeleteDC ( cdc );
  EndPaint ( hwnd, &ps );
  return 0;

}


LRESULT CALLBACK
wndproc ( HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam ) {

  if ( umsg == WM_DESTROY ) {
    PostQuitMessage ( 0 );
    return 0;
  }

  else if ( umsg == WM_PAINT ) {
    paint_w ( hwnd );
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

  else if ( umsg == WM_MOUSEMOVE ) {
    w.m.x = ( size_t ) LOWORD ( lparam );
    w.m.y = ( size_t ) HIWORD ( lparam );
    return 0;
  }

  else if ( umsg == WM_SIZE ) {
    w.width  = ( size_t ) LOWORD ( lparam );
    w.height = ( size_t ) HIWORD ( lparam );
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


#endif // W_H