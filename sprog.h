#ifndef SPROG_H
#define SPROG_H
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "err.h"
#include "sproge.h"


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
    SPROGE_PULSE_RECT
  };
  
  va_list vargs;
  va_start ( vargs, sprg );
  const sprogval_t arg_x = va_arg ( vargs, double );
  const sprogval_t arg_y = va_arg ( vargs, double );
  const sprogval_t arg_t = va_arg ( vargs, double );
  const sprogval_t arg_X = va_arg ( vargs, double );
  const sprogval_t arg_Y = va_arg ( vargs, double );
  va_end ( vargs );
  
  sprogstack_t *_stack = NULL;
  if ( sprg.pstack == NULL ) sprg.pstack = &_stack;
  #define SPROG_STACK ( *sprg.pstack )
  
  #define SPROG_POPA                         \
    if ( SPROG_STACK == NULL ) {             \
      SPROG_MSG_ENDA;                        \
      ERR ( ERR_SPROG_FAILED );              \
    }                                        \
    const sprogval_t a = SPROG_STACK->value; \
    prev = SPROG_STACK->prev;                \
    free ( SPROG_STACK );                    \
    SPROG_STACK = prev;
    
  #define SPROG_GETA                         \
    if ( SPROG_STACK == NULL ) {             \
      SPROG_MSG_ENDA;                        \
      ERR ( ERR_SPROG_FAILED );              \
    }                                        \
    const sprogval_t a = SPROG_STACK->value;
  
  #define SPROG_GETB                         \
    if ( SPROG_STACK == NULL ) {             \
      SPROG_MSG_ENDB;                        \
      ERR ( ERR_SPROG_FAILED );              \
    }                                        \
    const sprogval_t b = SPROG_STACK->value;
  
  #define SPROG_PUSH( v ) ({                         \
    prev = SPROG_STACK;                              \
    SPROG_STACK = calloc ( 1, sizeof *SPROG_STACK ); \
    SPROG_STACK->prev = prev;                        \
    SPROG_STACK->value = v;                          \
  })
  
  #define SPROG_FREESTACK ({          \
    for ( ; SPROG_STACK != NULL; ) {  \
      void *prev = SPROG_STACK->prev; \
      free ( SPROG_STACK );           \
      SPROG_STACK = prev;             \
    }                                 \
  })
  
  #ifdef ERR_MSG
    
    #define SPROG_MSG_ENDA ({                                \
      printf ( "sprogmsg: end of stack at command %c (%d) in \
channel %d, index %d\n", c, c, sprg.channel, i + 1 );        \
    })
  
    #define SPROG_MSG_ENDB ({                                    \
      printf ( "sprogmsg: end of stack at command (%f)%c (%d) in \
channel %d, index %d\n", a, c, c, sprg.channel, i + 1 );         \
    })
    
    #define SPROG_MSG_DIVB ({                                   \
        printf ( "sprogmsg: 0 division at (%f)(%f)%c in channel \
%d, index %d\n", b, a, c, sprg.channel, i + 1 );                \
    })
    
    #define SPROG_MSG_DUMP ({        \
      sprogstack_t *s = SPROG_STACK; \
      printf ( "sprogst: " );        \
      for ( ; s != NULL; ) {         \
        printf ( "(%f)", s->value ); \
        s = s->prev;                 \
      }                              \
      printf ( "\n" );               \
    })
    
    #define SPROG_MSG_RECOURSION ({                         \
        printf ( "sprogmsg: recoursion at (%f)%c in channel \
%d, index %d\n", a, c, sprg.channel, i + 1 );               \
    })

    #define SPROG_MSG_MAXDEPTH ({                                \
        printf ( "sprogmsg: maxdepth %d at (%f)%c in channel %d, \
index %d\n", sprg.depth, a, c, sprg.channel, i + 1 );            \
    })
    
    #define SPROG_MSG_UNKNOWNCOMM ({                        \
      printf ( "sprogmsg: unkown command %c (%d) in channel \
%d, index %d\n", c, c, sprg.channel, i + 1 );               \
    })
    
    #define SPROG_MSG_NOVALUE ({                     \
      printf ( "sprogmsg: channel %d leaves no value \
in stack at all\n", sprg.channel );                  \
    })
    
    #define SPROG_MSG_TOMANYVALUES ({               \
      printf ( "sprogmsg: channel %d leaves several \
values in stack\n", sprg.channel );                 \
    })
    
  #else // ERR_MSG
  
    #define SPROG_MSG_ENDA ({})
    #define SPROG_MSG_ENDB ({})
    #define SPROG_MSG_DIVB ({})
    #define SPROG_MSG_DUMP ({})
    #define SPROG_MSG_RECOURSION ({})
    #define SPROG_MSG_MAXDEPTH ({})
    #define SPROG_MSG_UNKNOWNCOMM ({})
    #define SPROG_MSG_NOVALUE ({})
    #define SPROG_MSG_TOMANYVALUES ({}) 
    
  #endif // ERR_MSG
  
  for ( size_t i = 0; programs[ sprg.channel ][ i ] > '\0'; ++i ) {
    const char c = programs[ sprg.channel ][ i ];
    void *prev = NULL;
    if ( c == ' ' ) {
      
    }
    else if ( c == 'x' ) {
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
      SPROG_STACK->value = a + b;
    }
    else if ( c == '-' ) {
      SPROG_POPA;
      SPROG_GETB;
      SPROG_STACK->value = a - b;
    }
    else if ( c == '%' ) {
      SPROG_POPA;
      SPROG_GETB;
      if ( b == 0 ) {
        SPROG_MSG_DIVB;
        ERR ( ERR_SPROG_FAILED );
      }
      SPROG_STACK->value = fmod ( a, b );
    }
    else if ( c == '*' ) {
      SPROG_POPA;
      SPROG_GETB;
      SPROG_STACK->value = a * b;
    }
    else if ( c == '~' ) {
      SPROG_POPA;
      SPROG_GETB;
      SPROG_STACK->value = ( ( int ) round ( a ) ) ^ ( ( int ) round ( b ) );
    }
    else if ( c == '<' ) {
      SPROG_POPA;
      SPROG_GETB;
      SPROG_STACK->value = a < b ? 1.0 : 0.0;
    }
    else if ( c == '^' ) {
      SPROG_POPA;
      SPROG_GETB;
      SPROG_STACK->value = pow ( a, b );
    }
    else if ( c == 'A' ) {
      SPROG_GETA;
      SPROG_STACK->value = fabs ( a );
    }
    else if ( c == '/' ) {
      SPROG_POPA;
      SPROG_GETB;
      if ( b == 0 ) {
        SPROG_MSG_DIVB;
        ERR ( ERR_SPROG_FAILED ); 
      }
      SPROG_STACK->value = a / b;
    }
    else if ( '0' <= c && c <= '9' || 'a' <= c && c <= 'f' ) {
      sprogval_t value;
      if ( '0' <= c && c <= '9' ) value = c - '0';
      else if ( 'a' <= c && c <= 'f' ) value = c - 'a' + 10;
      SPROG_PUSH ( value );
    }
    else if ( c == '@' ) {
      SPROG_POPA;
      const int ia = ( ( int ) round ( a ) );
      if ( ia == 0 ) {
        SPROG_MSG_RECOURSION;
        ERR ( ERR_SPROG_FAILED );  
      }
      if ( sprg.depth >= SPROG_MAX_DEPTH ) {
        SPROG_MSG_MAXDEPTH;
        ERR ( ERR_SPROG_FAILED );
      }
      sprog
        ( ( sprog_t )
              { sprg.channel + ia
              , sprg.depth + 1
              , &SPROG_STACK
              }
        , arg_x, arg_y, arg_t, arg_X, arg_Y 
        );
    }
    else {
      SPROG_MSG_UNKNOWNCOMM;
      SPROG_MSG_DUMP;
      SPROG_FREESTACK;
      ERR ( ERR_SPROG_FAILED );
    }
  }

  if ( sprg.depth > 0 ) return 0;
  
  if ( SPROG_STACK == NULL ) {
    SPROG_MSG_NOVALUE;
    ERR ( ERR_SPROG_FAILED );
  }
  
  const sprogval_t value = SPROG_STACK->value;
  
  if ( SPROG_STACK->prev != NULL ) {
    SPROG_MSG_TOMANYVALUES;
    SPROG_MSG_DUMP;
    SPROG_FREESTACK;
    ERR ( ERR_SPROG_FAILED );
  }
  
  SPROG_FREESTACK;
  
  return ( int ) ( 255 * value );
}






#endif // SPROG_H