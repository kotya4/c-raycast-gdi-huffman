// decodes imge and makes textures out of it
#ifndef IMGD_H
#define IMGD_H
#include <stdlib.h> // malloc
#include <math.h> // ceil, log2
#include "vectorbit.h"
#include "minmax.h"
#include "imge.h"
#include "imgn.h"
#include "bmpi.h"


typedef struct {
  bool transparent;
  bmpi_t **bmpis;
  int length;
  char type;
} texture_t;


#define TEXTURE_W( t, i ) ( ( t ).bmpis[ ( i ) ]->w )
#define TEXTURE_H( t, i ) ( ( t ).bmpis[ ( i ) ]->h )
#define TEXTURE_RGB24( t, i, k ) ( ( t ).bmpis[ ( i ) ]->p.rgb24[ ( t ).bmpis[ ( i ) ]->a[ ( k ) ] ] )


int
img_filterbytype
  ( const texture_t **o
  , const int o_length
  , const texture_t *ts
  , const int ts_length
  , const char type
  )
{
  for ( int i = 0; i < o_length; ++i ) {
    // pickup random texture from pull
    const texture_t *t = NULL;
    const int tioff = rand ();
    for ( int ti0 = 0; ti0 < ts_length; ++ti0 ) {
      const int ti = ( ti0 + tioff ) % ts_length;
      if ( ts[ ti ].type == type ) {
        t = &ts[ ti ];
        // check for duplicates
        for ( int k = 0; k < i; ++k ) {
          if ( o[ k ] == t ) {
            // cannot pick up diplicate texture
            t = NULL;
            break;
          }
        }
        // texture picked up
        if ( t != NULL ) break;
      }
    }
    if ( t == NULL ) {
      // cannot find any texture with given type
      // or o_length is bigger than ts_length
      return -1;
    }
    o[ i ] = t;
  }
  return 0;
}


int
img_decode ( bmpi_t **bmpis, int *bmpis_length, texture_t **ts, int *ts_length ) {

  vectorbit_t imagesizes =
    { imge_imagesizes
    , IMGE_IMAGESIZES_LENGTH
    , IMGE_IMAGESIZES_LENGTH - 1
    , VECTORBIT_BITSLEN ( IMGN_IMAGES_LENGTH, 4 )
    };

  vectorbit_t colorsnum =
    { imge_colorsnum
    , IMGE_COLORSNUM_LENGTH
    , IMGE_COLORSNUM_LENGTH - 1
    , VECTORBIT_BITSLEN ( IMGN_IMAGES_LENGTH, 4 )
    };

  vectorbit_t images =
    { imge_images
    , IMGE_IMAGES_LENGTH
    , IMGE_IMAGES_LENGTH - 1
    , VECTORBIT_BITSLEN ( IMGN_IMAGES_LENGTH, IMGE_IMAGES_BITSLENGTH )
    };

  int colors_index = IMGE_COLORS_LENGTH;

  *bmpis_length = IMGE_FILES_LENGTH;
  *bmpis = malloc ( *bmpis_length * sizeof **bmpis );

  for ( int i = 0; i < IMGE_FILES_LENGTH; ++i ) {

    // printf("imgd %d of %d\n", IMGE_FILES_LENGTH - i - 1, IMGE_FILES_LENGTH);

    bmpi_t *bmpi = &( *bmpis )[ IMGE_FILES_LENGTH - i - 1 ];
    bmpi_palette_t *p = &bmpi->p;

    int colors_length = vectorbit_pop_many ( &colorsnum, 4 );
    if ( colors_length < 0 ) return -1; // bad color length
    colors_length += 1;

    p->length = colors_length;
    p->rgb24 = malloc ( colors_length * sizeof *p->rgb24 );
    for ( int k = 0; k < colors_length; ++k ) {
      --colors_index;
      if ( colors_index < 0 ) return -2; // bad color index
      const short rgb332 = ( short ) imge_colors[ colors_index ];
      p->rgb24[ colors_length - k - 1 ].r = min ( 255, ( ( ( rgb332 >> 5 ) & 0b111 ) << 5 ) + 16 );
      p->rgb24[ colors_length - k - 1 ].g = min ( 255, ( ( ( rgb332 >> 2 ) & 0b111 ) << 5 ) + 16 );
      p->rgb24[ colors_length - k - 1 ].b = min ( 255, ( ( ( rgb332 >> 0 ) & 0b11  ) << 6 ) + 32 );
    }

    bmpi->h = vectorbit_pop_many ( &imagesizes, 2 );
    bmpi->w = vectorbit_pop_many ( &imagesizes, 2 );
    #define IMGD_PARSE_SIZE( s ) ({  \
      if      ( s == 0b00 ) s =  32; \
      else if ( s == 0b01 ) s =  64; \
      else if ( s == 0b10 ) s = 128; \
      else if ( s == 0b11 ) s = 256; \
      else return -1;                \
    })
    IMGD_PARSE_SIZE ( bmpi->w ); // incompatible file size at err
    IMGD_PARSE_SIZE ( bmpi->h );

    bmpi->l = bmpi->w * bmpi->h;
    bmpi->a = malloc ( bmpi->l * sizeof *bmpi->a );

    const int indexcap = ( int ) ceil ( log2 ( p->length ) );

    for ( int k = 0; k < bmpi->l; ++k ) {
      const int index = vectorbit_pop_many ( &images, indexcap );
      if ( 0 > index || index >= p->length ) return -4; // bad index
      bmpi->a[ bmpi->l - k - 1 ] = index;
    }

  }

  // TODO: do not store indices in fixed size arrays, compress with vectrobit

  *ts_length = IMGN_TEXTURES_LENGTH;
  *ts = malloc ( *ts_length * sizeof **ts );
  for ( int i = 0; i < IMGN_TEXTURES_LENGTH; ++i ) {
    texture_t *t = &( *ts )[ i ];
    t->transparent = false; // TODO
    t->type = imgn_type[ i ];
    t->bmpis = malloc ( IMGN_ARR_LENGTH * sizeof *t->bmpis );
    for ( t->length = 0; t->length < IMGN_ARR_LENGTH && imgn[ i ][ t->length ] > 0; ++t->length )
      t->bmpis[ t->length ] = &( *bmpis )[ imgn[ i ][ t->length ] - 1 ];
    void *oldptr = t->bmpis;
    t->bmpis = realloc ( t->bmpis, t->length * sizeof *t->bmpis );
    if ( t->bmpis == NULL ) {
      t->bmpis = oldptr;
      return -5; // realloc error
    }
  }

  return 0;
}






#endif // IMGD_H
