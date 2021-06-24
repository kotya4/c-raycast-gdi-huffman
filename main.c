#define UNICODE


#include <windows.h>
#include "debug.h"
#include "w.h"


int WINAPI
WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow ) {
  int status;
  
  
  HWND hwnd;
  init_hwnd ( &hwnd, L"ray", CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, hInstance );
  
  init_w ( hwnd );
  
  ShowWindow ( hwnd, nCmdShow );
  ShowCursor ( FALSE );

  MSG msg = {};
  while ( GetMessage( &msg, NULL, 0, 0 ) ) {
    TranslateMessage ( &msg );
    DispatchMessage ( &msg );
  }

  kill_w ();

  return msg.wParam;
  
}