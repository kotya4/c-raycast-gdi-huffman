#ifndef COLLISION_H
#define COLLISION_H
#include <stdbool.h>
#include "map.h"


int  // Returns 1 if outx/outy are updated.
collision_test
  ( const map_t *m
  , const double r   // collision radius
  , double nx        // new position
  , double ny
  , double *outx     // corrected position
  , double *outy
  )
{
  const int x = ( int ) nx;
  const int y = ( int ) ny;

  if ( MAP_SOLID ( *m, x, y ) ) return 0;

  const bool bt = MAP_SOLID ( *m, x, y - 1 );
  const bool bb = MAP_SOLID ( *m, x, y + 1 );
  const bool bl = MAP_SOLID ( *m, x - 1, y );
  const bool br = MAP_SOLID ( *m, x + 1, y );

  if ( bt && 0 - y + ny < r ) ny = 0 + y + r;
  if ( bb && 1 + y - ny < r ) ny = 1 + y - r;
  if ( bl && 0 - x + nx < r ) nx = 0 + x + r;
  if ( br && 1 + x - nx < r ) nx = 1 + x - r;

  const double r2 = r * r;

  if ( !( bt && bl ) && MAP_SOLID ( *m, x - 1, y - 1 ) ) {
    const double dx = nx - ( x + 0 );
    const double dy = ny - ( y + 0 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      ( qx > qy )
        ? ( nx = x + 0 + r )
        : ( ny = y + 0 + r );
  }

  if ( !( bt && br ) && MAP_SOLID ( *m, x + 1, y - 1 ) ) {
    const double dx = nx - ( x + 1 );
    const double dy = ny - ( y + 0 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      ( qx > qy )
        ? ( nx = x + 1 - r )
        : ( ny = y + 0 + r );
  }

  if ( !( bb && bb ) && MAP_SOLID ( *m, x - 1, y + 1 ) ) {
    const double dx = nx - ( x + 0 );
    const double dy = ny - ( y + 1 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      (qx > qy)
        ? ( nx = x + 0 + r )
        : ( ny = y + 1 - r );
  }

  if ( !( bb && br ) && MAP_SOLID ( *m, x + 1 , y + 1 ) ) {
    const double dx = nx - ( x + 1 );
    const double dy = ny - ( y + 1 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      (qx > qy)
        ? ( nx = x + 1 - r )
        : ( ny = y + 1 - r );
  }

  ( *outx ) = nx;
  ( *outy ) = ny;

  return 1;
}


#endif // COLLISION_H