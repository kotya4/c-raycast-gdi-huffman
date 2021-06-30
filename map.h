#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> // calloc
#include <string.h> // memset
#include "minmax.h"


#define MAP_TYPE_VOID 0 // can be door
#define MAP_TYPE_PASS 1 // can be door or void
#define MAP_TYPE_WALL 2
typedef uint16_t mapval_t;
// <ceiling:4><floor:4><wall:4><type:2> : 14
#define MAP_GET_TYPE( v ) ( ( v >> 0             ) & 0x3 )
#define MAP_GET_SURW( v ) ( ( v >> 0 + 2         ) & 0xf )
#define MAP_GET_SURF( v ) ( ( v >> 0 + 2 + 4     ) & 0xf )
#define MAP_GET_SURC( v ) ( ( v >> 0 + 2 + 4 + 4 ) & 0xf )
#define MAP_PUT_TYPE( v ) ( ( v & 0x3 ) << 0             )
#define MAP_PUT_SURW( v ) ( ( v & 0xf ) << 0 + 2         )
#define MAP_PUT_SURF( v ) ( ( v & 0xf ) << 0 + 2 + 4     )
#define MAP_PUT_SURC( v ) ( ( v & 0xf ) << 0 + 2 + 4 + 4 )
#define MAP_CHANGE_TYPE( m, v ) ({                                  \
  m = m ^ MAP_PUT_TYPE ( MAP_GET_TYPE ( m ) ) | MAP_PUT_TYPE ( v ); \
})

typedef struct {
  mapval_t *a;
  size_t w;
  size_t h;
  size_t l;
} map_t;


int
map_kill ( map_t *m ) {
  free ( m->a ); m->a = NULL;
}


int
map_init ( map_t *m, size_t w, size_t h ) {
  m->w = w;
  m->h = h;
  m->l = w * h;
  m->a = calloc ( m->l, sizeof *m->a );
}


int
map_surfacify // randomly changes existing surfaces
  ( map_t *m
  , const size_t x0
  , const size_t y0
  , const size_t w0
  , const size_t h0
  , const bool very_mazy
  )
{
  bool keep_wall_surface    = rand () % 2 > 0; // ~50%
  bool keep_floor_surface   = rand () % 4 > 0; // ~75%
  bool keep_ceiling_surface = rand () % 5 > 0; // ~80%
  if ( very_mazy ) {
    keep_wall_surface    = rand () % 10 > 0; // ~90%
    keep_floor_surface   = rand () % 10 > 0; // ~90%
    keep_ceiling_surface = rand () % 10 > 0; // ~90%
  }
  const size_t new_wall    = rand ();
  const size_t new_floor   = rand ();
  const size_t new_ceiling = rand ();
  for ( size_t y = y0; y < y0 + h0; ++y )
    for ( size_t x = x0; x < x0 + w0; ++x )
  {
    // get old surfaces
    size_t wall    = MAP_GET_SURW ( m->a[ y * m->w + x ] );
    size_t floor   = MAP_GET_SURF ( m->a[ y * m->w + x ] );
    size_t ceiling = MAP_GET_SURC ( m->a[ y * m->w + x ] );
    // flush old surfaces
    m->a[ y * m->w + x ] ^= MAP_PUT_SURW ( wall    );
    m->a[ y * m->w + x ] ^= MAP_PUT_SURF ( floor   );
    m->a[ y * m->w + x ] ^= MAP_PUT_SURC ( ceiling );
    // generate new surfaces
    if ( !keep_wall_surface    ) wall    = new_wall;
    if ( !keep_floor_surface   ) floor   = new_floor;
    if ( !keep_ceiling_surface ) ceiling = new_ceiling;
    // put new surfaces
    m->a[ y * m->w + x ] |= MAP_PUT_SURW ( wall    );
    m->a[ y * m->w + x ] |= MAP_PUT_SURF ( floor   );
    m->a[ y * m->w + x ] |= MAP_PUT_SURC ( ceiling );
  }
}


int
map_mazify // recoursive division maze
  ( map_t *m
  , size_t x0
  , size_t y0
  , size_t w0
  , size_t h0
  , size_t depth
  , size_t scaler
  )
{
  const size_t se = scaler << 1; // must be even because of odd doors
  { // skip if size is too small
    const size_t s = 5 + 2 * abs ( se );
    if ( w0 < s || h0 < s ) return 0;
  }
  // cross point
  const size_t px = x0 + 2 + se + ( rand () % max ( 1, w0 - ( 2 + se << 1 ) >> 1 ) << 1 );
  const size_t py = y0 + 2 + se + ( rand () % max ( 1, h0 - ( 2 + se << 1 ) >> 1 ) << 1 );
  // horisontal cross
  for ( size_t x = x0; x < x0 + w0; ++x ) {
    if ( MAP_GET_TYPE ( m->a[ py * m->w + x ] ) == MAP_TYPE_PASS ) {
      // WARN ( "door destroyed by hcross at depth %d, x=%d, y=%d\n", depth, x, py );
      MAP_CHANGE_TYPE ( m->a[ ( py - 1 ) * m->w + x ], MAP_TYPE_PASS );
      MAP_CHANGE_TYPE ( m->a[ ( py + 1 ) * m->w + x ], MAP_TYPE_PASS );
    }
    MAP_CHANGE_TYPE ( m->a [ py * m->w + x ], MAP_TYPE_WALL );
  }
  // vertical cross
  for ( size_t y = y0; y < y0 + h0; ++y ) {
    if ( MAP_GET_TYPE ( m->a[ y * m->w + px ] ) == MAP_TYPE_PASS ) {
      // WARN ( "door destroyed by vcross at depth %d, x=%d, y=%d\n", depth, px, y );
      MAP_CHANGE_TYPE ( m->a[ y * m->w + ( px - 1 ) ], MAP_TYPE_PASS );
      MAP_CHANGE_TYPE ( m->a[ y * m->w + ( px + 1 ) ], MAP_TYPE_PASS );
    }
    MAP_CHANGE_TYPE ( m->a [ y * m->w + px ], MAP_TYPE_WALL );
  }
  // fill surfaces
  map_surfacify ( m, x0, y0, w0, h0, scaler == 0 );
  // HACK: w/o " - scaler " wr/hb is bigger than need to and door
  //       can be placed into a border. need tests for scaler > 1.
  const size_t wl = px - x0 + 1;
  const size_t pl = ( rand () % ( wl - 1 >> 1 ) << 1 ) + 1;
  const size_t wr = w0 - ( px - x0 ) - scaler;
  const size_t pr = ( rand () % max ( 1, wr - 1 >> 1 ) << 1 ) + 1;
  const size_t ht = py - y0 + 1;
  const size_t pt = ( rand () % ( ht - 1 >> 1 ) << 1 ) + 1;
  const size_t hb = h0 - ( py - y0 ) - scaler;
  const size_t pb = ( rand () % max ( 1, hb - 1 >> 1 ) << 1 ) + 1;
  const size_t skip = rand () % 4; // add door on every room except one
  if ( skip != 0 ) MAP_CHANGE_TYPE ( m->a[ ( py ) * m->w + ( x0 + pl ) ], MAP_TYPE_PASS );
  if ( skip != 1 ) MAP_CHANGE_TYPE ( m->a[ ( py ) * m->w + ( px + pr ) ], MAP_TYPE_PASS );
  if ( skip != 2 ) MAP_CHANGE_TYPE ( m->a[ ( y0 + pt ) * m->w + ( px ) ], MAP_TYPE_PASS );
  if ( skip != 3 ) MAP_CHANGE_TYPE ( m->a[ ( py + pb ) * m->w + ( px ) ], MAP_TYPE_PASS );
  map_mazify ( m, x0, y0, wl, ht, depth + 1, scaler );
  map_mazify ( m, x0, py, wl, hb, depth + 1, scaler );
  map_mazify ( m, px, y0, wr, ht, depth + 1, scaler );
  map_mazify ( m, px, py, wr, hb, depth + 1, scaler );
}


int
map_borderify ( map_t *m ) {
  for ( size_t y = 0; y < m->h; ++y ) {
    MAP_CHANGE_TYPE ( m->a[ y * m->w + ( 0        ) ], MAP_TYPE_WALL );
    MAP_CHANGE_TYPE ( m->a[ y * m->w + ( m->w - 1 ) ], MAP_TYPE_WALL );
  }
  for ( size_t x = 0; x < m->w; ++x ) {
    MAP_CHANGE_TYPE ( m->a[ ( 0        ) * m->w + x ], MAP_TYPE_WALL );
    MAP_CHANGE_TYPE ( m->a[ ( m->h - 1 ) * m->w + x ], MAP_TYPE_WALL );
  }
}


int
map_crossify ( map_t *m ) {
  size_t w = m->w >> 1;
  size_t h = m->h >> 1;
  // cross
  for ( int y = 0; y < m->h; ++y ) MAP_CHANGE_TYPE ( m->a[ y * m->w + w ], MAP_TYPE_WALL );
  for ( int x = 0; x < m->w; ++x ) MAP_CHANGE_TYPE ( m->a[ h * m->w + x ], MAP_TYPE_WALL );
  // vertical doors
  for ( int i = rand () % 10 + 1; i >= 0; --i ) {
    const size_t y = ( rand () % h << 1 ) + 1;
    MAP_CHANGE_TYPE ( m->a[ y * m->w + w ], MAP_TYPE_PASS );
  }
  // horisontal doors
  for ( int i = rand () % 10 + 1; i >= 0; --i ) {
    const size_t x = ( rand () % w << 1 ) + 1;
    MAP_CHANGE_TYPE ( m->a[ h * m->w + x ], MAP_TYPE_PASS );
  }
}


#define MAP_MIRROR_NO     0b00
#define MAP_MIRROR_PASTE  0b01
#define MAP_MIRROR_MIRROR 0b10
int
map_mirrorify ( map_t *m, const size_t mx, const size_t my ) {
  for ( size_t y = 0; y < m->h >> 1; ++y ) {
    size_t y0 = y;
    if ( my & MAP_MIRROR_PASTE ) y0 = ( m->h >> 1 ) + y;
    if ( my & MAP_MIRROR_MIRROR ) y0 = m->h - y - 1;
    for ( size_t x = 0; x < m->w >> 1; ++x ) {
      size_t x0 = x;
      if ( mx & MAP_MIRROR_PASTE ) x0 = ( m->w >> 1 ) + x;
      if ( mx & MAP_MIRROR_MIRROR ) x0 = m->w - x - 1;
      m->a[ y0 * m->w + x0 ] = m->a[ y * m->w + x ];
    }
  }
  if ( rand () % 4 > 0 ) // 75%
    map_surfacify
      ( m
      , mx & MAP_MIRROR_NO ? 0 : m->w >> 1
      , my & MAP_MIRROR_NO ? 0 : m->h >> 1
      , m->w >> 1
      , m->h >> 1
      , false
      );
}


int
map_voidify ( map_t *m, size_t x, size_t y, const size_t w, const size_t h ) {
  for ( ; y < h; ++y )
    for ( ; x < w; ++x )
      m->a[ y * w + x ] = 0;
}


int
map_quadrantify ( map_t *m ) { // quadrant maze generator
  const size_t w = ( m->w >> 1 ) + ( rand () & 1 );
  const size_t h = ( m->h >> 1 ) + ( rand () & 1 );
  size_t quadrants = 0b000; // <TR><BL><BR>
  const size_t offseti = rand () % 3;
  for ( size_t i = 0; i < 3 && quadrants != 0b111; ++i ) {
    map_voidify ( m, 0, 0, w, h );
    map_mazify ( m, 0, 0, w, h, 0, rand () % 8 > 0 ); // catacombs 12.5%, rooms 87.5%
    // choose quadrants and mirror to them
    size_t currect_quadrants = ( 1 << ( i + offseti ) % 3 );
    currect_quadrants |= ( rand () % 2 << 2 ) | ( rand () % 2 << 1 ) | ( rand () % 2 << 0 );
    currect_quadrants ^= quadrants;
    quadrants |= currect_quadrants;
    if ( currect_quadrants & 0b001 ) // BR
      map_mirrorify
        ( m
        , rand () % 2 == 0 ? MAP_MIRROR_MIRROR : MAP_MIRROR_PASTE
        , rand () % 2 == 0 ? MAP_MIRROR_MIRROR : MAP_MIRROR_PASTE
        );
    if ( currect_quadrants & 0b010 ) // BL
      map_mirrorify
        ( m
        , rand () % 2 == 0 ? MAP_MIRROR_MIRROR : MAP_MIRROR_PASTE
        , MAP_MIRROR_NO
        );
    if ( currect_quadrants & 0b100 ) // TR
      map_mirrorify
        ( m
        , MAP_MIRROR_NO
        , rand () % 2 == 0 ? MAP_MIRROR_MIRROR : MAP_MIRROR_PASTE
        );
  }
  if ( rand () % 2 == 0 ) { // TL
    map_voidify ( m, 0, 0, w, h );
    map_mazify ( m, 0, 0, w, h, 0, rand () % 8 > 0 ); // catacombs 12.5%, rooms 87.5%
  }
}


#endif // MAP_H