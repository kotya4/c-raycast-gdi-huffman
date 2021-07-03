// Known bugs:
// cast_walls produses ugly mirroring artefact ;
#define UNICODE
#include <windows.h>
#include "w.h"


int WINAPI
WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow ) {
  int status;

  const int w = 640;
  const int h = 480;
  HWND hwnd;
  make_hwnd ( &hwnd, L"ray", CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, hInstance );

  const int dw = 300;
  const int dh = ( int )( dw * h / ( double ) w );
  w_init ( hwnd, dw, dh );

  ShowWindow ( hwnd, nCmdShow );
  ShowCursor ( FALSE );
  center_mouse ( hwnd );


  MSG msg = {};
  while ( GetMessage( &msg, NULL, 0, 0 ) ) {
    TranslateMessage ( &msg );
    DispatchMessage ( &msg );
  }

  w_kill ();

  return msg.wParam;

}