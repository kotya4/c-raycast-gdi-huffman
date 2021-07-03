#ifndef SOUND_H
#define SOUND_H
#include "audio.h"


void
sound_kbjorklu ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    ( ( t * ( t >> 8 | t >> 9 ) & 46 & t >> 8 ) ) ^ ( t & t >> 13 | t >> 6 ) // kbjorklu
  ;
}


void
sound_FortyTwoMelody ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    ( t << 1 ) * ( 21 & t >> 11 )
  ;
}


void
sound_FortyTwoMelodyWestern ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t*((42&t>>10)%14) // Western Forty-Two Melody
  ;
}


void
sound_E3G3C4D4 ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t*(5+((t>>11)&5)) // E3, G3, C4 and D4
  ;
}


void
sound_melodygenerator ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    ( t ^ t - 2 ) // melody generator
  ;
}


void
sound_NiklasRoy ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t*(t>>9|t>>13)&16 // Niklas Roy's
  ;
}


void
sound_SierpinskiHarmony ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t&t>>8 // Sierpinski Harmony
  ;
}


void
sound_miiro ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t*5&(t>>7)|t*3&(t*4>>10) // (from miiro)
  ;
}


void
sound_viznut ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    (t*5&t>>7)|(t*3&t>>10) // (from viznut)
  ;
}


void
sound_stephth ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t*9&t>>4|t*5&t>>7|t*3&t/1024 // (from stephth)
  ;
}


void
sound_ambientdrone ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t&t%255 // ambient drone
  ;
}


void
sound_Rrrola ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t*(0xCA98>>(t>>9&14)&15)|t>>8 // Rrrola's
  ;
}


void
sound_Mu6k_bassinput ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    (t>>8^t>>10|t>>14|(t))&63 // Mu6k's  bass input
  ;
}


void
sound_serpinski8bitdrum ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    (t&t>>4)-5 // serpinski 8-bit drum
  ;
}


void
sound_serpinskimodularwraparound ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    (t*9&t>>4|t*5&t>>7|t*3&t/1024)-1 // serpinski modular wrap-around
  ;
}


void
sound_droid ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t>>4
    |t
    &((t>>5)
    / ( (t>>7-(t>>15)&-t>>7-(t>>15)) | 1 )  ) // from droid
  ;
}


void
sound_bst ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    (int)(t/1e7*t*t+t)%127|t>>4|t>>5|t%127+(t>>16)|t // from bst
  ;
}


void
sound_droid2 ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t>>6&1?t>>5:-t>>4 // from droid (2)
  ;
}


void
sound_Mu6k ( audiobuffer_t *b, const int secs ) {
  audiobuffer_init ( b, secs * 8000 );
  for ( int t = 0; t < b->l; ++t ) b->a[ t ] =
    t>>6^t>>8|t>>12|t&63 // Mu6k's
  ;
}


typedef void ( *soundf_t )( audiobuffer_t *, const int );


soundf_t soundfuncs[] = {
  &sound_kbjorklu,
  &sound_FortyTwoMelody,
  &sound_FortyTwoMelodyWestern,
  &sound_E3G3C4D4,
  &sound_melodygenerator,
  &sound_NiklasRoy,
  &sound_SierpinskiHarmony,
  &sound_miiro,
  &sound_viznut,
  &sound_stephth,
  &sound_ambientdrone,
  &sound_Rrrola,
  &sound_Mu6k_bassinput,
  &sound_serpinski8bitdrum,
  &sound_serpinskimodularwraparound,
  &sound_droid,
  &sound_bst,
  &sound_droid2,
  &sound_Mu6k,
};


#endif // SOUND_H
