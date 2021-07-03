// some usefull types
#ifndef T_H
#define T_H
#include <stdbool.h>


typedef struct {
  int x;
  int y;
  bool locked;
} mouse_t;


typedef struct {
  size_t id;
  size_t delayms;
} timer_t;


typedef struct {
  bool side;
  int mapx;
  int mapy;
  int ti;
  const void *t; // type of texture_t *
} depthstack_t;
#define DEPTHSTACK_LENGTH 2 // depth of transparency


#endif // T_H
