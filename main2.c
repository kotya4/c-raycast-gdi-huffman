#define UNICODE
#include <windows.h>
#include "w2.h"


int WINAPI
WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow ) {
  int status;

  const int w = 100;
  const int h = 100;
  HWND hwnd;
  make_hwnd ( &hwnd, L"ray", CW_USEDEFAULT, CW_USEDEFAULT, w, h, hInstance );

  const int dw = 100;
  const int dh = 100;
  w_init ( hwnd, dw, dh );

  ShowWindow ( hwnd, nCmdShow );
//  ShowCursor ( FALSE );
  //center_mouse ( hwnd );

  MSG msg = {};
  while ( GetMessage( &msg, NULL, 0, 0 ) ) {
    TranslateMessage ( &msg );
    DispatchMessage ( &msg );
  }

  w_kill ();

  return msg.wParam;

}
