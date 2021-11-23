#ifndef W_H
#define W_H
#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> // calloc
#include <string.h> // memset, memcpy
#include <time.h> // time
#include "warn.h"
// #include "sprog.h"
#include "map.h"
#include "img.h"
#include "audio.h"
#include "sound.h"
#include "camera.h"
#include "font.h"
#include "sprintfexp.h"
#include "cast.h"
#include "typedefs.h"
#include "display.h"
#include "dibbuffer.h"
#include "collision.h"
#include "entity.h"
#include "husk.h"

typedef struct {

  mouse_t m;

  bool keys[ 256 ];

  timer_t render_timer;

  int time; // posix in millisecs
  int time0; // program running in millisecs
  int elapsed; // time elapsed between frames

  display_t d;

  dibbuffer_t dib;

  int width;
  int height;

  // =====================================
  // GAME ; TODO: maybe need to be separated
  // =====================================

  map_t map;

  bmpi_t *bmpis;
  int bmpis_length;

  texture_t *textures;
  int textures_length;

  audio_t audio;

  #define W_SOUNDS_LENGTH 1
  audiobuffer_t sounds[ W_SOUNDS_LENGTH ];

  camera_t camera;

  font_t font;

  const texture_t *surs[ MAP_SURFACES_LENGTH ]; // array of pointers to wall surfaces

  entity_t *entities;
  int entities_length;

} window_t;
// HACK: wndproc arguments list is predefined so any optional value
//       passed to the winproc must be scoped globally.
window_t w;


int
w_kill () {
  display_kill ( &w.d );
  dibbuffer_kill ( &w.dib );
  map_kill ( &w.map );
  for ( int i = 0; i < w.bmpis_length; ++i ) bmpi_kill ( &w.bmpis[ i ] );
  w.bmpis = NULL;
  audio_kill ( &w.audio );
  for ( int i = 0; i < W_SOUNDS_LENGTH; ++i ) audiobuffer_kill ( &w.sounds[ i ] );
  font_kill ( &w.font );
  for ( int i = 0; i < MAP_SURFACES_LENGTH; ++i ) w.surs[ i ] = NULL;
  free ( w.entities ); w.entities = NULL;
  return 0;
}


int
w_init ( HWND hwnd, int dw, int dh ) {

  int randseed = time ( NULL );
  WARN ( "randseed: %d\n", randseed );
  srand ( randseed );

  w.m = ( mouse_t ) { 0, 0, true };

  w.render_timer.id = 1;
  w.render_timer.delayms = 1000 / 60; // 60 frames per second

  w.time = GetTickCount ();
  w.time0 = 0;

  memset ( w.keys, false, 256 * sizeof *w.keys );

  display_init ( &w.d, dw, dh );

  dibbuffer_init ( &w.dib, dw, dh, GetDC ( hwnd ) );

  w.width  = 0; // will be init on WM_SIZE after window appear
  w.height = 0;

  map_init ( &w.map, 33, 33 );
  map_quadrantify ( &w.map );
  map_crossify ( &w.map );
  map_borderify ( &w.map );

  map_animshiftify ( &w.map );


  w.camera = CAMERA_DEFAULT;
  {
    const int offset = rand ();
    for ( int i0 = 0; i0 < w.map.l; ++i0 ) {
      const int i = ( i0 + offset ) % w.map.l;
      if ( MAP_GET_TYPE ( w.map.a[ i ] ) != MAP_TYPE_WALL ) {
        w.camera.x = i % w.map.w + 0.5;
        w.camera.y = i / w.map.w + 0.5;
        break;
      }
    }
  }

  img_decode ( &w.bmpis, &w.bmpis_length, &w.textures, &w.textures_length );

  font_init ( &w.font, w.textures[ IMGN_Ifixedsysa ].bmpis[ 0 ], 128, 192, 16, 16 );

  img_filterbytype ( w.surs, MAP_SURFACES_LENGTH, w.textures, w.textures_length, 'S' );

  // for ( int i = 0; i < 16; ++i ) w.surs[ i ] = &w.textures[ IMGN_Sme ];

  audio_init ( &w.audio );
  // sound_kbjorklu ( &w.sounds[ 0 ], 60 );
  // ( *soundfuncs[ rand () % ( sizeof soundfuncs / sizeof *soundfuncs ) ] )( &w.sounds[ 0 ], 60 );
  // audiobuffer_volume ( &w.sounds[ 0 ], 0.75 );
  // audiobuffer_fadein ( &w.sounds[ 0 ], 10 );
  // audio_play ( &w.audio, &w.sounds[ 0 ] );


  w.entities_length = 1;
  w.entities = malloc ( w.entities_length * sizeof *w.entities );
  w.entities[ 0 ] = ( entity_t ) { &w.textures[ IMGN_Erock ], 0, w.camera.x, w.camera.y };
  w.textures[ IMGN_Erock ].transparent = true;

  return 0;
}



//int huskstri=0;
//char huskstr[128]="";
//int huskstrupdated=0;

void
OnPress(char key){
	husk_onpress(key);
	//huskstr[huskstri]=key;
	//huskstri++;
	//if(huskstri>=128){
	//	huskstri=0;
	//}
	//huskstrupdated=1;
}



int
w_paint ( HWND hwnd ) {
  {
    const int time = GetTickCount ();
    w.elapsed = time - w.time;
    w.time0 += w.elapsed;
    w.time = time;
  }
  PAINTSTRUCT ps = {};
  HDC hdc = BeginPaint ( hwnd, &ps );
  const int wt = ps.rcPaint.top;
  const int wl = ps.rcPaint.left;
  const int ww = ps.rcPaint.right - ps.rcPaint.left;
  const int wh = ps.rcPaint.bottom - ps.rcPaint.top;
  const int dw = w.d.width;
  const int dh = w.d.height;
  uint8_t *p = w.d.pixels;
  memset ( p, 0, w.d.length * sizeof *p );
  memset ( w.d.depthbuffer, 0, w.d.width * w.d.height * sizeof *w.d.depthbuffer );

/*

  cast_floor ( &w.d, &w.map, &w.camera, w.surs, 1, 10.0, CAST_FLOOR | CAST_CEILING );

  cast_walls ( &w.d, &w.map, &w.camera, w.surs, 15, 16.0 );

  for ( int i = 0; i < w.entities_length; ++i ) {
    cast_entity ( &w.d, &w.camera, &w.entities[ i ], 16.0 );
  }


  static int colors[ 16 ];
  static bool colors_gen = true;
  if ( colors_gen ) {
    colors_gen = false;
    for ( int i = 0; i < 16; ++i ) {
      colors[ i ] = rand () % 0x1000000;
    }
  }
  for ( int y = 0; y < w.map.h; ++y )
    for ( int x = 0; x < w.map.w; ++x )
  {
    int type = MAP_GET_TYPE ( w.map.a[ y * w.map.w + x ] );
    int surs = MAP_GET_SURW ( w.map.a[ y * w.map.w + x ] );
    int surf = MAP_GET_SURF ( w.map.a[ y * w.map.w + x ] );
    int surc = MAP_GET_SURC ( w.map.a[ y * w.map.w + x ] );
    if ( type == MAP_TYPE_WALL ) {
      p[ ( y * dw + x ) * 3 + 2 ] = ( uint8_t ) ( colors[ surs ] >> 0x00 ) & 0xff;
      p[ ( y * dw + x ) * 3 + 1 ] = ( uint8_t ) ( colors[ surs ] >> 0x08 ) & 0xff;
      p[ ( y * dw + x ) * 3 + 0 ] = ( uint8_t ) ( colors[ surs ] >> 0x10 ) & 0xff;
    }
    else {
      p[ ( y * dw + x ) * 3 + 2 ] = ( uint8_t ) ( ( colors[ surc ] >> 0x00 ) & 0xff ) / 2;
      p[ ( y * dw + x ) * 3 + 1 ] = ( uint8_t ) ( ( colors[ surc ] >> 0x08 ) & 0xff ) / 2;
      p[ ( y * dw + x ) * 3 + 0 ] = ( uint8_t ) ( ( colors[ surc ] >> 0x10 ) & 0xff ) / 2;
    }
  }

  p[ ( ( int ) w.camera.y * dw + ( int ) w.camera.x ) * 3 + 2 ] = ( uint8_t ) 0xff;
  p[ ( ( int ) w.camera.y * dw + ( int ) w.camera.x ) * 3 + 1 ] = ( uint8_t ) 0xf0;
  p[ ( ( int ) w.camera.y * dw + ( int ) w.camera.x ) * 3 + 0 ] = ( uint8_t ) 0x00;

*/

// MY HUSK START
{
//	static int cx=0;//cursor pos
//	static int cy=0;
// p -- pixelbuffer?
//	static int color=0xff0000;//red or blue?
	//static char str[128]="hello";

//	if(huskstrupdated){
//		huskstrupdated=0;
//	}

//	font_paint(&w.font,p,dw,dh,cx,cy,huskstr,color);

	husk_ondraw(&w.font,p,dw,dh);

}
// MY HUSK END

  font_paint ( &w.font, p, dw, dh, 0, dh - 12, sprintf128 ( "e:%d", w.elapsed ), 0xff00ff );

  memcpy ( w.dib.hbmpraw, p, w.d.length * sizeof *p );
  dibbuffer_flush ( &w.dib, hdc, wl, wt, ww, wh, 0, 0, dw, dh );
  EndPaint ( hwnd, &ps );
  return 0;
}


int
center_mouse ( HWND hwnd ) {
  POINT point = { w.width / 2, w.height / 2 };
  ClientToScreen ( hwnd, &point );
  SetCursorPos ( point.x, point.y );
  return 0;
}


int
w_update ( HWND hwnd ) {



/*
  const int mdx = w.m.x - w.width  / 2;
  const int mdy = w.m.y - w.height / 2;
  center_mouse ( hwnd );
  if ( mdx != 0 ) {
    camera_rotate ( &w.camera, mdx * -0.0001 * w.elapsed );
  }


  if ( w.keys[ 'W' ] ) {
    const double ncx = w.camera.x + w.camera.dirx * +0.004 * w.elapsed;
    const double ncy = w.camera.y + w.camera.diry * +0.004 * w.elapsed;
    collision_test ( &w.map, 0.25, ncx, ncy, &w.camera.x, &w.camera.y );
  } else if ( w.keys[ 'S' ] ) {
    const double ncx = w.camera.x + w.camera.dirx * -0.004 * w.elapsed;
    const double ncy = w.camera.y + w.camera.diry * -0.004 * w.elapsed;
    collision_test ( &w.map, 0.25, ncx, ncy, &w.camera.x, &w.camera.y );
  }
  if ( w.keys[ 'A' ] ) {
    const double ncx = w.camera.x + w.camera.planex * -0.004 * w.elapsed;
    const double ncy = w.camera.y + w.camera.planey * -0.004 * w.elapsed;
    collision_test ( &w.map, 0.25, ncx, ncy, &w.camera.x, &w.camera.y );
  } else if ( w.keys[ 'D' ] ) {
    const double ncx = w.camera.x + w.camera.planex * +0.004 * w.elapsed;
    const double ncy = w.camera.y + w.camera.planey * +0.004 * w.elapsed;
    collision_test ( &w.map, 0.25, ncx, ncy, &w.camera.x, &w.camera.y );
  }

  if ( w.keys[ 'P' ] ) {
    w.entities[ 0 ].x = w.camera.x;
    w.entities[ 0 ].y = w.camera.y;
  }



  static int anim_i = 0;
  anim_i += 1;
  if ( anim_i > 2 ) {
    map_animate ( &w.map );
    anim_i = 0;
  }
*/
  return 0;
}



LRESULT CALLBACK
wndproc ( HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam ) {

  if ( umsg == WM_DESTROY ) {
    PostQuitMessage ( 0 );
    return 0;
  }

  else if ( umsg == WM_PAINT ) {
    w_paint ( hwnd );
    return 0;
  }

  else if ( umsg == WM_KEYDOWN ) {
	OnPress((uint8_t)wparam);
    w.keys[ ( uint8_t ) wparam ] = true;
    if ( wparam == VK_ESCAPE ) { // exit on escape
      //PostQuitMessage ( 0 );
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
      // HACK: w_update cannot be called from WM_PAINT or mouse will not work
      w_update ( hwnd );
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
    w.m.x = ( int ) LOWORD ( lparam );
    w.m.y = ( int ) HIWORD ( lparam );
    return 0;
  }

  else if ( umsg == WM_SIZE ) {
    w.width  = ( int ) LOWORD ( lparam );
    w.height = ( int ) HIWORD ( lparam );
    return 0;
  }

  return DefWindowProc ( hwnd, umsg, wparam, lparam );
}


int
make_hwnd
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

  if ( *hwnd == NULL ) {
    WARN ( "CreateWindowEx: hwnd is null\n" );
    return -1;
  }

  return 0;
}


#endif // W_H
