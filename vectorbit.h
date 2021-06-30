#ifndef VECTORBIT_H
#define VECTORBIT_H
#include <stdlib.h>
#include <stdint.h>


typedef struct {
  uint8_t *array;
  size_t capacity; // number of allocated bytes
  size_t byteindex; // index of byte in use
  size_t bitslength; // number of pushed bits in last byte
} vectorbit_t;


// calculates " bitslength " by number of values aka
// " size " and number of bits per value aka " bits ".
#define VECTORBIT_BITSLEN( size, bits ) ( size * bits % 8 ? size * bits % 8 : 8 )


int
vectorbit_push ( vectorbit_t *v, const size_t value ) {
  if ( v == NULL ) return -1; // vectorbit is NULL
  if ( v->array == NULL ) {
    v->capacity = 0;
    v->byteindex = 0;
    v->bitslength = 0;
  }
  if ( v->bitslength + 1 > 8 ) {
    // number of pushed bits in byte bigger than one byte can store
    v->byteindex += 1;
    v->bitslength = 0;
  }
  if ( v->byteindex == v->capacity ) {
    // allocate one more byte
    void *oldptr = v->array;
    v->array = realloc ( v->array, ( v->capacity + 1 ) * sizeof *v->array );
    if ( !v->array ) {
      v->array = oldptr;
      return -2; // realloc returns NULL
    }
    v->capacity += 1;
    // zero allocated memory
    v->array[ v->byteindex ] = 0;
  }
  // store bit in array
  v->array[ v->byteindex ] |= ( ( value & 1 ) << v->bitslength );
  ++v->bitslength;
}


int
vectorbit_push_many ( vectorbit_t *v, const size_t value, const size_t num ) {
  // TODO: rewrite with precalculated reallocation
  for ( size_t i = 0; i < num; ++i ) {
    int status;
    status = vectorbit_push ( v, ( value >> i & 1 ) );
    if ( status < 0 ) return status;
  }
}


int // returns 0 or 1 on success or <0 if any error occures
vectorbit_pop ( vectorbit_t *v ) {
  if ( v == NULL ) return -1; // vectorbit is NULL
  if ( v->array == NULL ) return -2; // vectorbit array is NULL
  if ( v->bitslength == 0 ) {
    if ( v->byteindex == 0 ) return -3; // vectorbit byteindex is 0
    --v->byteindex;
    v->bitslength = 8;
  }
  --v->bitslength;
  int value = v->array[ v->byteindex ] >> v->bitslength & 1;
  // v->array[ v->byteindex ] ^= value << v->bitslength; // flush
  return value;
}


int
vectorbit_pop_many ( vectorbit_t *v, const size_t num ) {
  int value = 0;
  for ( size_t i = 0; i < num; ++i ) {
    int ret;
    ret = vectorbit_pop ( v );
    if ( ret < 0 ) return ret;
    value |= ret << ( num - i - 1 );
  }
  return value;
}


#endif // VECTORBIT_H
