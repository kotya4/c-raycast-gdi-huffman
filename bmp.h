#ifndef BMP_H
#define BMP_H
#include <stdbool.h>
#include <stdlib.h> // malloc
#include <stdio.h> // fopen
#include "bmpi.h"


typedef struct {
  size_t l;
  size_t w;
  size_t h;
  unsigned char info [ 54 ];
  unsigned char *bgr;
} bmp_t;


int
bmp_kill ( bmp_t *o ) {
  free ( o->bgr ); o->bgr = NULL;
}


int
bmp_from_file ( bmp_t *o, const char *path ) {
  FILE *f = fopen ( path, "rb" );
  if ( f == NULL ) return -1; // fopen returns NULL
  fread ( o->info, sizeof *o->info, 54, f ); // read the 54-byte header
  o->w = *( size_t* ) &o->info[ 18 ];
  o->h = *( size_t* ) &o->info[ 22 ]; // can be -1
  // Width and height must be power of 2, nor file format
  // will be incompatible with reading algorithm used here.
  o->l = 3 * o->w * o->h; // allocate 3 bytes per pixel
  o->bgr = malloc ( o->l * sizeof *o->bgr );
  fread ( o->bgr, sizeof *o->bgr, o->l, f );
  fclose ( f );
}


int
bmp_mirror ( bmp_t *o ) {
  for ( size_t y = 0; y < o->h >> 1; ++y )
    for ( size_t x = 0; x < o->w; ++x )
  {
    const size_t j = o->w * ( o->h - 1 - y ) + x;
    const size_t k = o->w * y + x;
    const unsigned char b = o->bgr[ j * 3 + 0 ];
    const unsigned char g = o->bgr[ j * 3 + 1 ];
    const unsigned char r = o->bgr[ j * 3 + 2 ];
    o->bgr [ j * 3 + 0 ] = o->bgr[ k * 3 + 0 ];
    o->bgr [ j * 3 + 1 ] = o->bgr[ k * 3 + 1 ];
    o->bgr [ j * 3 + 2 ] = o->bgr[ k * 3 + 2 ];
    o->bgr[ k * 3 + 0 ] = b;
    o->bgr[ k * 3 + 1 ] = g;
    o->bgr[ k * 3 + 2 ] = r;
  }
}


int
bmp_to_bmpi ( const bmp_t *bmp, bmpi_t *bmpi, const size_t max_p_length ) {
  // max_p_length ignored if 0
  const size_t p_step = 16; // how many memory will be added at once
  bmpi->w = bmp->w;
  bmpi->h = bmp->h;
  bmpi->l = bmpi->w * bmpi->h;
  bmpi->a = malloc ( bmpi->l * sizeof *bmpi->a );
  bmpi->p.length = 0;
  bmpi->p.rgb24 = NULL;
  size_t *aptr = bmpi->a;
  size_t actual_p_length = 0;
  for ( size_t i = 0; i < bmp->l; i += 3 ) {
    const bmpi_rgb24_t rgb24 =
      { bmp->bgr[ i + 2 ]
      , bmp->bgr[ i + 1 ]
      , bmp->bgr[ i + 0 ]
      };
    size_t index;
    bool found = false; // find same color in palette
    for ( index = 0; index < actual_p_length; ++index ) {
      if ( BMPI_RGB24_EQUAL ( bmpi->p.rgb24[ index ], rgb24 ) ) {
        found = true;
        break;
      }
    }
    if ( !found ) {
      if ( max_p_length > 0 && actual_p_length + 1 > max_p_length ) {
        bmpi_kill ( bmpi );
        return -1; // palette index bigger than palette max index
      }
      if ( bmpi->p.length <= actual_p_length ) { // expands palette
        bmpi->p.length += p_step;
        void *oldptr = bmpi->p.rgb24;
        bmpi->p.rgb24 =
          realloc ( bmpi->p.rgb24, bmpi->p.length * sizeof *bmpi->p.rgb24 );
        if ( !bmpi->p.rgb24 ) {
          bmpi->p.rgb24 = oldptr;
          bmpi_kill ( bmpi );
          return -2; // realloc returns NULL at indexating
        }
      }
      index = actual_p_length;
      bmpi->p.rgb24[ index ] = rgb24;
      ++actual_p_length;
    }
    *aptr = index;
    ++aptr;
  }
  if ( bmpi->p.length != actual_p_length ) { // shrink to fit
    bmpi->p.length = actual_p_length;
    void *oldptr = bmpi->p.rgb24;
    bmpi->p.rgb24 = realloc ( bmpi->p.rgb24, bmpi->p.length * sizeof *bmpi->p.rgb24 );
    if ( !bmpi->p.rgb24 ) {
      bmpi->p.rgb24 = oldptr;
      bmpi_kill ( bmpi );
      return -3; // realloc returns NULL at shrinking
    }
  }
}











#endif // BMP_H
