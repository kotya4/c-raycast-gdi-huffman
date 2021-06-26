#define UNICODE


#include <windows.h>
#include "w.h"


int WINAPI
WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow ) {
  int status;
  
  HWND hwnd;
  init_hwnd ( &hwnd, L"ray", CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, hInstance );
  
  init_w ( hwnd, 64, 64 );
  
  ShowWindow ( hwnd, nCmdShow );
  // ShowCursor ( FALSE );

  MSG msg = {};
  while ( GetMessage( &msg, NULL, 0, 0 ) ) {
    TranslateMessage ( &msg );
    DispatchMessage ( &msg );
  }

  kill_w ();

  return msg.wParam;
  
}