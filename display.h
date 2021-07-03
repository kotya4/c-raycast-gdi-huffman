#ifndef DISPLAY_H
#define DISPLAY_H


typedef struct {
  int width;
  int height;
  int area; // width * height
  int length; // area * 3
  unsigned char *pixels; // size of length
  double *depthbuffer; // size of area
} display_t;


int
display_kill ( display_t *d ) {
  free ( d->pixels ); d->pixels = NULL;
  free ( d->depthbuffer ); d->depthbuffer = NULL;
  return 0;
}


int
display_init ( display_t *d, const int w, const int h ) {
  d->width = w;
  d->height = h;
  d->area = d->width * d->height;
  d->length = d->area * 3;
  d->pixels = calloc ( d->length, sizeof *d->pixels );
  d->depthbuffer = calloc ( d->area, sizeof *d->depthbuffer );
  return 0;
}


#endif // DISPLAY_H
