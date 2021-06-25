#ifndef SPROG_H
#define SPROG_H
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "err.h"


typedef double sprogval_t;


typedef struct {
  sprogval_t value;
  void *prev;
} sprogstack_t;


typedef struct {
  size_t channel;
  size_t depth;
  sprogstack_t **pstack;
} sprog_t;

#define SPROG_RED   ( sprog_t ) { 0, 0, NULL }
#define SPROG_GREEN ( sprog_t ) { 1, 0, NULL }
#define SPROG_BLUE  ( sprog_t ) { 2, 0, NULL }

typedef const char * sprogp_t;

#define SPROG_MAX_DEPTH 256

int
sprog ( sprog_t sprg, ... ) {
  static const sprogp_t programs[] = {
    // circle dance
    "a1/ 2x21/-^   2y13t%--^ +<",
    "a1/ 2x13t%--^ 2y21/-^   +<",
    "a1/ 2xX-^     2yY-^     +<",
    // empty
    "0",
    "0",
    "0",
    // xy*
    "1@2@*",
    "x",
    "y",
    // maxdepth error
    "x1@+",
    "y10-@",
    // circle dance
    "a1/ 2x21/-^   2y13t%--^ +<",
    "a1/ 2x13t%--^ 2y21/-^   +<",
    "a1/ 2xX-^     2yY-^     +<",
    // move with mouse
    "a1/ 2xX-^     2yY-^     +<",
    //                (t%3-1-y)^2
    "a1/ 2x21/-^      2y13t%--^      +<",
    //radius  (0.5-x)^2   (0.5-y)^2   circle
    "a1/      2x21/-^     2y21/-^     +   <",
    // some serpinski masked wit circle quad
    "tff*x*ff*y*~ff**/ 2xy+/1-1xx*yy*+<* *",
    // circle quad masks sum
    "2xy+/1- 1xx*yy*+< *",
    // scaled circle quad
    "a1/xx*yy*+<",
  };
  
  sprogstack_t *stack;
  if ( sprg.depth > 0 ) {
    stack = *sprg.pstack;
  } else {
    stack = NULL;
  }
  
  
  va_list vargs;
  va_start ( vargs, sprg );
  const sprogval_t arg_x = va_arg ( vargs, double );
  const sprogval_t arg_y = va_arg ( vargs, double );
  const sprogval_t arg_t = va_arg ( vargs, double );
  const sprogval_t arg_X = va_arg ( vargs, double );
  const sprogval_t arg_Y = va_arg ( vargs, double );
  va_end ( vargs );
  
  #define SPROG_POPA                                           \
    if ( stack == NULL ) {                                     \
      printf ( "sprog: end of stack at command %c (%d)         \
      in channel %d, index %d\n", c, sprg.channel, i + 1 );   \
      ERR ( -5 );                                              \
    }                                                          \
    const sprogval_t a = stack->value;                         \
    prev = stack->prev;                                        \
    free ( stack );                                            \
    stack = prev;
  
  #define SPROG_GETB                                            \
    if ( stack == NULL ) {                                      \
      printf ( "sprog: end of stack at command (%f)%c (%d)      \
      in channel %d, index %d\n", a, c, sprg.channel, i + 1 ); \
      ERR ( -5 );                                               \
    }                                                           \
    const sprogval_t b = stack->value;
  
  #define SPROG_PUSH( v ) ({             \
    prev = stack;                        \
    stack = calloc ( 1, sizeof *stack ); \
    stack->prev = prev;                  \
    stack->value = v;                    \
  })
  size_t i;
  for ( i = 0; programs[ sprg.channel ][ i ] > '\0'; ++i ) {
    const char c = programs[ sprg.channel ][ i ];
    void *prev = NULL;
    if ( c == 'x' ) {
      SPROG_PUSH( arg_x );
    }
    else if ( c == 'y' ) {
      SPROG_PUSH( arg_y );
    }
    else if ( c == 't' ) {
      SPROG_PUSH( arg_t );
    }
    else if ( c == 'X' ) {
      SPROG_PUSH( arg_X );
    }
    else if ( c == 'Y' ) {
      SPROG_PUSH( arg_Y );
    }
    else if ( c == '+' ) {
      SPROG_POPA;
      SPROG_GETB;
      stack->value = a + b;
    }
    else if ( c == '-' ) {
      SPROG_POPA;
      SPROG_GETB;
      stack->value = a - b;
    }
    else if ( c == '%' ) {
      SPROG_POPA;
      SPROG_GETB;
      if ( b == 0 ) {
        printf ( "sprog: 0 division at (%f)(%f)%c \
        in channel %d, index %d\n", b, a, c, sprg.channel, i + 1 ); 
        ERR ( -6 );    
      }
      stack->value = fmod ( a, b );
    }
    else if ( c == '*' ) {
      SPROG_POPA;
      SPROG_GETB;
      stack->value = a * b;
    }
    else if ( c == '~' ) {
      SPROG_POPA;
      SPROG_GETB;
      stack->value = ( ( int ) round ( a ) ) ^ ( ( int ) round ( b ) );
    }
    else if ( c == '<' ) {
      SPROG_POPA;
      SPROG_GETB;
      stack->value = a < b ? 1.0 : 0.0;
    }
    else if ( c == '^' ) {
      SPROG_POPA;
      SPROG_GETB;
      stack->value = pow ( a, b );
    }
    else if ( c == '/' ) {
      SPROG_POPA;
      SPROG_GETB;
      if ( b == 0 ) {
        printf ( "sprog: 0 division at (%f)(%f)%c \
        in channel %d, index %d\n", b, a, c, sprg.channel, i + 1 ); 
        ERR ( -6 );    
      }
      stack->value = a / b;
    }
    else if ( '0' <= c && c <= '9' || 'a' <= c && c <= 'f' ) {
      prev = stack;
      stack = calloc ( 1, sizeof *stack );
      stack->prev = prev;
      if ( '0' <= c && c <= '9' ) {
        stack->value = c - '0';
      }
      else if ( 'a' <= c && c <= 'f' ) {
        stack->value = c - 'a' + 10;
      }
    }
    else if ( c == ' ' ) {
      
    }
    else if ( c == '@' ) {
      SPROG_POPA;
      const int ia = ( ( int ) round ( a ) );
      if ( ia == 0 ) {
        printf ( "sprog: recoursion at (%f)%c \
        in channel %d, index %d\n", a, c, sprg.channel, i + 1 ); 
        ERR ( -7 );    
      }
      if ( sprg.depth >= SPROG_MAX_DEPTH ) {
        printf ( "sprog: maxdepth %d at (%f)%c \
        in channel %d, index %d\n", sprg.depth, a, c, sprg.channel, i + 1 ); 
        ERR ( -8 ); 
      }
      sprg.pstack = &stack;
      sprog
        ( ( sprog_t )
              { sprg.channel + ia
              , sprg.depth + 1
              , sprg.pstack
              }
        , arg_x, arg_y, arg_t, arg_X, arg_Y 
        );
      stack = *sprg.pstack;
    }
    else {
      printf ( "sprog: unkown command %c (%d) \
      in channel %d, index %d\n", c, sprg.channel, i + 1 );
      printf ( "stack: " );
      for ( ; stack != NULL; ) {
        printf ( "(%d)", stack->value );
        void *prev = stack->prev;
        free ( stack );
        stack = prev;
      }
      printf ( "\n" );
      ERR ( -2 );
    }
  }

  if ( sprg.depth > 0 ) {
    *sprg.pstack = stack;
    return 0;
  }

  if ( stack == NULL ) {
    printf ( "sprog: channel %d leaves no value in stack at all\n", sprg.channel );
    ERR ( -3 );
  }
  
  const sprogval_t value = stack->value;
  
  if ( stack->prev != NULL ) {
    printf ( "sprog: channel %d leaves values in stack after %f\n", sprg.channel, value );
    printf ( "stack: " );
    for ( ; stack->prev != NULL; ) {
      void *prev = stack->prev;
      free ( stack );
      stack = prev;
      printf ( "(%f)", stack->value );
    }
    printf ( "\n" );
    ERR ( -4 );
  }
  
  free ( stack );
  
  return ( int ) ( 255 * value );
}






#endif // SPROG_H