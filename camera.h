#ifndef CAMERA_H
#define CAMERA_H
#include <math.h>


typedef struct {
  double rotation;
  double dirx;
  double diry;
  double planex;
  double planey;
  double x;
  double y;
} camera_t;


#define CAMERA_DEFAULT ( ( camera_t ) { 0.0, -1.0, 0.0, 0.0, 0.66, 0.0, 0.0 } )


int
camera_rotate ( camera_t *c, const double drot ) {
  c->rotation += drot;
  const double cosa = cos ( drot );
  const double sina = sin ( drot );
  const double dirx_o = c->dirx;
  const double diry_o = c->diry;
  c->dirx = dirx_o * cosa - diry_o * sina;
  c->diry = dirx_o * sina + diry_o * cosa;
  const double planex_o = c->planex;
  const double planey_o = c->planey;
  c->planex = planex_o * cosa - planey_o * sina;
  c->planey = planex_o * sina + planey_o * cosa;
}


#endif // CAMERA_H
