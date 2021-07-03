#ifndef FONT_H
#define FONT_H
#include <stdbool.h>
#include <stdlib.h> // malloc
#include <stdarg.h>
#include "bmpi.h"


typedef struct {
  size_t cw; // pixels in character
  size_t ch;
  size_t tw; // characters in table
  size_t th;
  bool **table; // intensity mask ( 0 for no color )
} font_t;


int
font_kill ( font_t *f ) {
  for ( int i = 0; i < f->tw * f->th; ++i ) free ( f->table [ i ] );
  free ( f->table ); f->table = NULL;
  return 0;
}


int
font_init
  ( font_t *f
  , bmpi_t *b
  , const size_t w   // used pixels in table
  , const size_t h
  , const size_t tw  // characters in table
  , const size_t th
  )
{
  f->cw = w / tw; // pixels in character
  f->ch = h / th;
  f->tw = tw;
  f->th = th;
  f->table = malloc ( tw * th * sizeof *f->table );
  for ( size_t n = 0; n < th; ++n ) {
    for ( size_t m = 0; m < tw; ++m ) {
      const size_t ci = tw * n + m;
      f->table[ ci ] = malloc ( f->cw * f->ch * sizeof **f->table );
      for ( size_t y = 0; y < f->ch; ++y ) {
        for ( size_t x = 0; x < f->cw; ++x ) {
          const size_t bi = w * f->ch * n + f->cw * m + w * y + x;
          f->table [ ci ] [ f->cw * y + x ] =
            ! BMPI_RGB24_EQUAL ( BMPI_RGB24_ALPHA, b->p.rgb24[ b->a[ bi ] ] );
        }
      }
    }
  }
  return 0;
}


int
font_paint
  ( const font_t *f
  , unsigned char *pixels
  , const size_t w
  , const size_t h
  , size_t x0
  , const size_t y0
  , const char *text
  , const size_t rgb8
  )
{
  for ( ; *text != '\0'; ++text ) {
    for ( size_t y = 0; y < f->ch; ++y ) {
      if ( y0 + y >= h ) break;
      else if ( y0 + y < 0 ) continue;
      for ( size_t x = 0; x < f->cw; ++x ) {
        if ( x0 + x >= w ) break;
        else if ( x0 + x < 0 ) continue;
        if ( f->table[ ( unsigned char ) ( *text ) ][ y * f->cw + x ] ) {
          const size_t pi = ( ( y0 + y ) * w + ( x0 + x ) ) * 3;
          pixels[ pi + 2 ] = ( unsigned char ) ( ( rgb8 >> 0x10 ) & 0xff );
          pixels[ pi + 1 ] = ( unsigned char ) ( ( rgb8 >> 0x08 ) & 0xff );
          pixels[ pi + 0 ] = ( unsigned char ) ( ( rgb8 >> 0x00 ) & 0xff );
        }
      }
    }
    x0 += f->cw;
    if ( x0 >= w ) break;
  }
  return 0;
}



#endif // FONT_H
