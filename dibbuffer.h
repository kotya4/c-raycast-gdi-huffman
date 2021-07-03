#ifndef DIBBUFFER_H
#define DIBBUFFER_H
#include <windows.h>


typedef struct {
  BITMAPINFOHEADER bmpih;
  BITMAPINFO bmpi;
  void *hbmpraw;
  HBITMAP hbmp;
} dibbuffer_t;


int
dibbuffer_kill ( dibbuffer_t *b ) {
  DeleteObject ( b->hbmp );
  b->hbmpraw = NULL;
  return 0;
}


int
dibbuffer_init ( dibbuffer_t *b, const int w, const int h, HDC hdc ) {
  b->bmpih = ( BITMAPINFOHEADER )
    { sizeof ( BITMAPINFOHEADER ) // header size
    , w
    , -h          // negative for top-down DIB
    , 1           // planes, must be 1
    , 24          // 3 bytes per pixel, bmiColors can be NULL
    , BI_RGB      // w\o compression
    , 0           // image size, can be 0 for BI_RGB
    , 0           // unused by CreateDIBSection
    , 0           // unused by CreateDIBSection
    , 0           // used colors, must be 0 for packed bmps
    , 0           // required colors, can be 0
    };
  b->bmpi = ( BITMAPINFO ) { b->bmpih, { [ 0 ] = ( RGBQUAD ) {} } };
  b->hbmpraw = NULL;
  // HACK: idk must i store bmpi because i send it not by value so i store them all
  b->hbmp = CreateDIBSection ( hdc, &b->bmpi, DIB_RGB_COLORS, &b->hbmpraw, NULL, 0 );
  return 0;
}


int
dibbuffer_flush
  ( dibbuffer_t *b
  , HDC hdc
  , const int wleft
  , const int wtop
  , const int wwidth
  , const int wheight
  , const int dleft
  , const int dtop
  , const int dwidth
  , const int dheight
  )
{
  HDC cdc = CreateCompatibleDC ( hdc );
  HBITMAP oldhbmp = ( HBITMAP ) SelectObject ( cdc, b->hbmp );
  StretchBlt ( hdc, wleft, wtop, wwidth, wheight, cdc, 0, 0, dwidth, dheight, SRCCOPY );
  SelectObject ( cdc, oldhbmp );
  DeleteDC ( cdc );
  return 0;
}


#endif // DIBBUFFER_H
