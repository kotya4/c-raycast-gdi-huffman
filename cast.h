#ifndef CAST_H
#define CAST_H
#include <math.h> // fabs
#include "warn.h"
#include "minmax.h"
#include "camera.h"
#include "map.h"
#include "img.h"
#include "display.h"
#include "typedefs.h"
#include "entity.h"


int
cast_walls
  ( display_t *d
  , const map_t *m
  , const camera_t *c
  , const texture_t **surfaces
  , const int render_distance
  , const double brightness
  )
{
  for ( int px = 0; px < d->width; ++px ) {
    // Display x in camera space.
    const double cx = 2.0 * px / d->width - 1;
    const double raydirx = c->dirx + c->planex * cx;
    const double raydiry = c->diry + c->planey * cx;
    // Length of ray from one x or y-side to next x or y-side.
    const double deltax = fabs ( 1.0 / raydirx );
    const double deltay = fabs ( 1.0 / raydiry );
    // Which box of the map we're in.
    int mapx = ( int ) c->x;
    int mapy = ( int ) c->y;
    if ( mapx < 0 || mapy < 0 ) {
      WARN ( "one of map indices < 0 ( %d, %d )\n", mapx, mapy );
      return -1;
    }
    // Length of ray from current position to next x or y-side.
    double sidedistx, sidedisty;
    // What direction to step in x or y-direction (either +1 or -1).
    int stepx;
    int stepy;
    // Calculating step and initial side_dist.
    if ( raydirx < 0 ) { stepx = -1; sidedistx = -deltax * ( mapx - c->x     ); }
    else               { stepx = +1; sidedistx = +deltax * ( mapx - c->x + 1 ); }
    if ( raydiry < 0 ) { stepy = -1; sidedisty = -deltay * ( mapy - c->y     ); }
    else               { stepy = +1; sidedisty = +deltay * ( mapy - c->y + 1 ); }
    // Performing DDA.
    int stack_length = 0;
    depthstack_t stack[ DEPTHSTACK_LENGTH ];
    for ( int di = 0; di < render_distance; ++di ) {
      bool side;
      if ( sidedistx < sidedisty ) { sidedistx += deltax; mapx += stepx; side = false; }
      else                         { sidedisty += deltay; mapy += stepy; side = true;  }
      // Was there a wall hit?
      if ( MAP_SOLID ( *m, mapx, mapy ) ) {
        // append values to depth stack
        const mapval_t mv = m->a[ mapy * m->w + mapx ];
        const texture_t *t = surfaces[ MAP_GET_SURW ( mv ) ];
        const int ti = MAP_GET_SI ( mv ) % t->length;
        stack[ stack_length ].side = side;
        stack[ stack_length ].mapx = mapx;
        stack[ stack_length ].mapy = mapy;
        stack[ stack_length ].t = t;
        stack[ stack_length ].ti = ti;
        ++stack_length;
        if ( stack_length >= DEPTHSTACK_LENGTH ) break;
        if ( !t->transparent ) break;
      }
    }
    // draw depth stack from backward
    for ( int si = stack_length - 1; si >= 0; --si ) {
      const bool side = stack[ si ].side;
      const int mapx = stack[ si ].mapx;
      const int mapy = stack[ si ].mapy;
      const texture_t *t = stack[ si ].t;
      const int ti = stack[ si ].ti;
      // Distance of perpendicular ray ( euclidean distance will give fisheye effect )
      const double perpdist =
        side
          ? ( mapy - c->y + 0.5 - stepy / 2.0 ) / raydiry
          : ( mapx - c->x + 0.5 - stepx / 2.0 ) / raydirx;
      // Height of strip to draw on screen.
      const int strip_height = d->height / perpdist;
      // Lowest and highest pixel to fill in current strip.
      const int strip_y0 = max ( 0,         ( d->height - strip_height ) / 2 );
      const int strip_y1 = min ( d->height, ( d->height + strip_height ) / 2 );
      // How much to increase the texture coordinate per screen pixel?
      const double tstep =  TEXTURE_W ( *t, ti ) / ( double ) strip_height;
      // Starting texture coordinate.
      double tpos = ( strip_y0 - d->height / 2.0 + strip_height / 2.0 ) * tstep;
      // Where exactly the wall was hit?
      double texx =
        side
          ? ( c->x + perpdist * raydirx )
          : ( c->y + perpdist * raydiry );
      texx -= ( int ) texx;
      // Texture x coordinate
      int tx = ( int )( TEXTURE_W ( *t, ti ) * texx );
      if ( side ^ raydirx > 0 ) tx = TEXTURE_W ( *t, ti ) - tx - 1;
      // Fade out by distance
      double fade = min ( 1.0, max ( 0.1, 1 - perpdist / brightness ) );
      if ( side ) fade /= 2.0;
      // draw vertical strip
      for ( int py = strip_y0; py < strip_y1; ++py ) {
        // Cast the texture coordinate to integer,
        // and mask with (texture_height - 1) in case of overflow.
        const int ty = ( int ) tpos & ( TEXTURE_H ( *t, ti ) - 1 );
        tpos += tstep;
        // Apply pixels to the display.
        const int ati = TEXTURE_W ( *t, ti ) * ty + tx;
        const bmpi_rgb24_t rgb24 = TEXTURE_RGB24 ( *t, ti, ati );
        // skip alpha
        if ( t->transparent && BMPI_RGB24_EQUAL ( rgb24, BMPI_RGB24_ALPHA ) ) {
          continue;
        }
        // set pixels
        d->pixels[ ( d->width * py + px ) * 3 + 2 ] = ( uint8_t )( rgb24.r * fade );
        d->pixels[ ( d->width * py + px ) * 3 + 1 ] = ( uint8_t )( rgb24.g * fade );
        d->pixels[ ( d->width * py + px ) * 3 + 0 ] = ( uint8_t )( rgb24.b * fade );
        // set depthbuffer
        d->depthbuffer[ d->width * py + px ] = perpdist;
      }
    }
  }
  return 0;
}


#define CAST_FLOOR   0b01
#define CAST_CEILING 0b10
int
cast_floor
  ( display_t *d
  , const map_t *m
  , const camera_t *c
  , const texture_t **surfaces
  , const int floorlimit      // blank part of floor/ceiling from screen center in pixels
  , const double brightness
  , const int flags
  )
{
  for ( int py = 0; py < d->height / 2 - floorlimit; ++py ) {
    // ray_dir for leftmost ray (x = 0) and rightmost ray (x = w)
    const double raydirx0 = c->dirx - c->planex;
    const double raydiry0 = c->diry - c->planey;
    const double raydirx1 = c->dirx + c->planex;
    const double raydiry1 = c->diry + c->planey;
    // Vertical position of the camera.
    const double posz = 0.5 * d->height;
    // Current y position compared to the center of the screen (the horizon).
    const double p = d->height / 2.0 - py;
    // Horizontal distance from the camera to the floor for the current row.
    // 0.5 is the z position exactly in the middle between floor and ceiling.
    const double rowdist = posz / ( p ? p : 1 );
    // calculate the real world step vector we have to add for each x (parallel to camera plane)
    // adding step by step avoids multiplications with a weight in the inner loop
    const double stepx = rowdist * ( raydirx1 - raydirx0 ) / d->width;
    const double stepy = rowdist * ( raydiry1 - raydiry0 ) / d->width;
    // Fade out by distance.
    const double fade = min ( 1.0, max ( 0.1, 1 - rowdist / brightness ) );
    // Real world coordinates of the leftmost column. This will be updated as we step to the right.
    double floorx = c->x + rowdist * raydirx0;
    double floory = c->y + rowdist * raydiry0;
    for( int px = 0; px < d->width; ++px ) {
      // The cell coord is simply got from the integer parts of floorx and floory.
      const int mapx = ( int ) floorx;
      const int mapy = ( int ) floory;
      // Updating real world coordinates.
      floorx += stepx;
      floory += stepy;
      if ( flags & CAST_CEILING > 0 ) {
        // Texture
        const mapval_t mv = m->a[ mapy * m->w + mapx ];
        const texture_t *t = surfaces[ MAP_GET_SURC ( mv ) ];
        const int ti = MAP_GET_SI ( mv ) % t->length;
        const int tw = TEXTURE_W ( *t, ti );
        const int th = TEXTURE_H ( *t, ti );
        // Gets the texture coordinate from the fractional part.
        const int tx = ( int ) ( tw * ( floorx - mapx ) ) & ( tw - 1 );
        const int ty = ( int ) ( th * ( floory - mapy ) ) & ( th - 1 );
        const int ati = tw * ty + tx;
        const bmpi_rgb24_t rgb24 = TEXTURE_RGB24 ( *t, ti, ati );
        if ( ! ( t->transparent && BMPI_RGB24_EQUAL ( rgb24, BMPI_RGB24_ALPHA ) ) ) {
          // set pixels
          d->pixels[ ( d->width * py + px ) * 3 + 2 ] = ( uint8_t )( rgb24.r * fade );
          d->pixels[ ( d->width * py + px ) * 3 + 1 ] = ( uint8_t )( rgb24.g * fade );
          d->pixels[ ( d->width * py + px ) * 3 + 0 ] = ( uint8_t )( rgb24.b * fade );
        }
      }
      if ( flags & CAST_FLOOR > 0 ) {
        // Texture
        const mapval_t mv = m->a[ mapy * m->w + mapx ];
        const texture_t *t = surfaces[ MAP_GET_SURF ( mv ) ];
        const int ti = MAP_GET_SI ( mv ) % t->length;
        const int tw = TEXTURE_W ( *t, ti );
        const int th = TEXTURE_H ( *t, ti );
        // Gets the texture coordinate from the fractional part.
        const int tx = ( int ) ( tw * ( floorx - mapx ) ) & ( tw - 1 );
        const int ty = ( int ) ( th * ( floory - mapy ) ) & ( th - 1 );
        const int ati = tw * ty + tx;
        const bmpi_rgb24_t rgb24 = TEXTURE_RGB24 ( *t, ti, ati );
        if ( ! ( t->transparent && BMPI_RGB24_EQUAL ( rgb24, BMPI_RGB24_ALPHA ) ) ) {
          // set pixels
          d->pixels[ ( d->width * ( d->height - py - 1 ) + px ) * 3 + 2 ] = ( uint8_t )( rgb24.r * fade );
          d->pixels[ ( d->width * ( d->height - py - 1 ) + px ) * 3 + 1 ] = ( uint8_t )( rgb24.g * fade );
          d->pixels[ ( d->width * ( d->height - py - 1 ) + px ) * 3 + 0 ] = ( uint8_t )( rgb24.b * fade );
        }
      }
    }
  }
  return 0;
}




int
cast_entity
  ( display_t *d
  , const camera_t *c
  , entity_t *e
  , const double brightness
  )
{
  // _________     __________   __________     [ camera.planex   camera.dirx ] ( -1 )
  // transform = ( sprite.pos - camera.pos ) x |                             |
  //                                           [ camera.planey   camera.diry ]
  const double det = 1.0 / ( c->planex * c->diry - c->dirx * c->planey );
  const double rx = e->x - c->x;
  const double ry = e->y - c->y;
  const double transformx = det * ( rx * c->diry - ry * c->dirx );
  const double transformy = det * ( ry * c->planex     - rx * c->planey );
  if ( transformy < 0 ) return 0; // out of near plane
  const double scale = 0.5;
  const int spritex = ( int ) ( d->width / 2 * ( 1 + transformx / transformy ) );
  const int spriteh0 = abs ( ( int ) ( d->height / transformy ) );
  const int spriteh = ( int ) ( spriteh0 * scale );
  const int spritew = spriteh;
  const int sdy = ( spriteh0 - spriteh ) / 2;
  const int spritey0 = max ( d->height / 2 - spriteh / 2 + sdy, 0 );
  const int spritey1 = min ( d->height / 2 + spriteh / 2 + sdy, d->height - 1 );
  const int spritex0 = max ( spritex - spritew / 2, 0 );
  const int spritex1 = min ( spritex + spritew / 2, d->width - 1 );
  const double dist = sqrt ( rx * rx + ry * ry );
  // Fade out by distance
  const double fade = min ( 1.0, max ( 0.1, 1 - ( dist / brightness ) ) );
  const int tw = TEXTURE_W ( *e->t, e->ti );
  const int th = TEXTURE_H ( *e->t, e->ti );
  for ( int px = spritex0; px < spritex1; ++px ) {
    const int tx = ( int )( ( px - spritex + spritew / 2 ) * tw / ( double ) spritew );
    for ( int py = spritey0; py < spritey1; ++py ) {
      const double dbuf = d->depthbuffer[ d->width * py + px ];
      if ( dbuf != 0 && dbuf < dist ) continue;
      const int ty = ( py - spritey0 ) * th / spriteh;
      // Apply pixels to the display.
      const int ati = tw * ty + tx;
      const bmpi_rgb24_t rgb24 = TEXTURE_RGB24 ( *e->t, e->ti, ati );
      // skip alpha
      if ( e->t->transparent && BMPI_RGB24_EQUAL ( rgb24, BMPI_RGB24_ALPHA ) ) {
        continue;
      }
      // set pixels
      d->pixels[ ( d->width * py + px ) * 3 + 2 ] = ( uint8_t )( rgb24.r * fade );
      d->pixels[ ( d->width * py + px ) * 3 + 1 ] = ( uint8_t )( rgb24.g * fade );
      d->pixels[ ( d->width * py + px ) * 3 + 0 ] = ( uint8_t )( rgb24.b * fade );
      // depth
      d->depthbuffer[ d->width * py + px ] = dist || 1;
    }
  }
  return 0;
}


#endif // CAST_H
