#include <stdio.h>
#include <stdlib.h>


typedef struct {

  int v;
  void *p;

} s_t;


int
s_new ( s_t **sp, int v ) {
  s_t *p = *sp;
  *sp = malloc ( 1 * sizeof **sp );
  (*sp)->v = v;
  (*sp)->p = p;
}


// int
// s_new_many ( s_t **s, int depth ) {
  // s_t *p = *s;
  // *s = malloc ( 1 * sizeof **s );
  // (*s)->v = v;
  // (*s)->p = p;
// }


int
s_dump ( s_t **sp ) {
  s_t *s = *sp;
  for ( ; s != NULL ; ) {
    printf ( "%d %p\n", s->v, s->p );
    s = s->p;
  }
}


int
s_free ( s_t **sp ) {
  s_t *s = *sp;
  for ( ; s != NULL ; ) {
    s_t *p = s->p;
    free ( s );
    s = p;
  }
}


int
main () {

  printf ( "test\n" );

  // typedef unsigned int t;
  // typedef t * tt;

  const int imgn[ 3 ][ 3 ] = { { 33, }, { 0, 1, }, { 19, 2, 3, } };


  // printf ( "%d\n", imgn );
  // printf ( "%d\n", *imgn );
  printf ( "%d\n", imgn[0][1] );


  return 0;



  printf ( "%d = %d\n", 0b11 << 1 + 1, 0b11 << ( 1 + 1 ) );



  return 0;

  int d = 1;

  s_t *s = NULL;

  s_new ( &s, 666 );
  s_new ( &s, 777 );

  s_dump ( &s );



  s_free ( &s );
}



// int
// f2 ( int dd, int d, s_t *os, s_t **out ) {
  // printf ( "\ndepth: %d\n", d );
  // s_t *s = os;
  // printf ( os == NULL ? "s was NULL\n" : "s was POINTER\n" );

  // {
    // void *p = s;
    // s = malloc ( 1 * sizeof *s );
    // s->v = d;
    // s->p = p;
    // printf ( "allocated new s\n" );
  // }

  // printf("s dump:\n");
  // {
    // s_t *dump = s;
    // for ( int i = 0; dump != NULL; ++i ) {
      // printf ( "v:%d i:%d p:", dump->v, i );
      // printf ( dump->p == NULL ? "NULL\n" : "POINTER\n" );
      // void *p = dump->p;
      // dump = p;
    // }
  // }

  // // if ( out != NULL )

  // if ( d <= 0 ) {
    // *out = s;
    // return 0;
  // }

  // s_t *in;
  // f ( dd, d - 1, s, &in );
  // s = in;

  // // s = *out;

  // printf("\nafter return at depth %d:\n", d);
  // {
    // s_t *dump = s;
    // for ( int i = 0; dump != NULL; ++i ) {
      // printf ( "v:%d i:%d p:", dump->v, i );
      // printf ( dump->p == NULL ? "NULL\n" : "POINTER\n" );
      // void *p = dump->p;
      // dump = p;
    // }
  // }

  // if ( dd == d ) {
    // printf("\nfreed\n");
    // for ( int i = 0; s != NULL; ++i ) {
      // void *p = s->p;
      // free ( s );
      // s = p;
    // }
  // }
// }
