#ifndef BMPI_H
#define BMPI_H
#include <stdlib.h>


typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} bmpi_rgb24_t;


#define BMPI_RGB24_EQUAL( c1, c2 ) ( c1.r == c2.r && c1.g == c2.g && c1.b == c2.b )


const bmpi_rgb24_t BMPI_RGB24_ALPHA = { 144, 240, 224 };


typedef struct {
  size_t length;
  bmpi_rgb24_t *rgb24;
} bmpi_palette_t;


typedef struct {
  size_t w;
  size_t h;
  size_t l;
  size_t *a;
  bmpi_palette_t p;
} bmpi_t;


int
bmpi_kill ( bmpi_t *o ) {
  free ( o->p.rgb24 ); o->p.rgb24 = NULL;
  free ( o->a ); o->a = NULL;
}


bmpi_rgb24_t *
bmpi_rgb24_at ( const bmpi_t *o, size_t x, size_t y ) {
  if ( x >= o->w || y >= o->h ) return NULL;
  return & ( o->p.rgb24[ o->a[ y * o->w + x ] ] );
}


#endif // BMPI_H
