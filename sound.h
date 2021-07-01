#ifndef SOUND_H
#define SOUND_H
#include "audio.h"


int
sound_kbjorklu ( audiobuffer_t *b ) {
  audiobuffer_init ( b, 60 * 8000 );

  int x, y;

  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    // ( ( t * ( t >> 8 | t >> 9 ) & 46 & t >> 8 ) ) ^ ( t & t >> 13 | t >> 6 ) // kbjorklu

    // ( t << 1 ) * ( 21 & t >> 11 ) // Forty-Two Melody

    // t*((42&t>>10)%14) // Western Forty-Two Melody

    // t*(5+((t>>11)&5)) // E3, G3, C4 and D4

    // ( t ^ t - 2 ) // melody generator

    // t*(t>>9|t>>13)&16 // Niklas Roy's

    // t&t>>8 // Sierpinski Harmony

    // t*5&(t>>7)|t*3&(t*4>>10) // (from miiro)

    // (t*5&t>>7)|(t*3&t>>10) // (from viznut)

    t*9&t>>4|t*5&t>>7|t*3&t/1024 // (from stephth)

    // t&t%255 // ambient drone

    // t*(0xCA98>>(t>>9&14)&15)|t>>8 // Rrrola's

    // (t>>8^t>>10|t>>14|(t))&63 // Mu6k's  bass input

    // (t&t>>4)-5 // serpinski 8-bit drum

    // (t*9&t>>4|t*5&t>>7|t*3&t/1024)-1 // serpinski modular wrap-around


/*
    t>>4

    |t

    &((t>>5)

    / ( (t>>7-(t>>15)&-t>>7-(t>>15)) | 1 )  ) // from droid
  */

  // (int)(t/1e7*t*t+t)%127|t>>4|t>>5|t%127+(t>>16)|t // from bst


  // t>>6&1?t>>5:-t>>4 // from droid (2)

    // t>>6^t>>8|t>>12|t&63 // Mu6k's

    ;
}


int
sound_fadein ( audiobuffer_t *b, const size_t secs ) {
  const size_t len = secs * 8000;
  for ( size_t t = 0; t < b->l && t < len; ++t )
    b->a[ t ] = b->a[ t ] * ( t / ( double ) len );
}


#endif // SOUND_H
