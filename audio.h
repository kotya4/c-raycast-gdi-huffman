#ifndef AUDIO_H
#define AUDIO_H
#include <windows.h>
#include <stdlib.h>
#include <stdint.h>


// WAVEHDR->lpData is char, but unsignedness
// is essential for calculations in sound.h
typedef struct {
  size_t l;
  unsigned char *a;
} audiobuffer_t;


int
audiobuffer_kill ( audiobuffer_t *b ) {
  free ( b->a ); b->a = NULL;
}


int
audiobuffer_init ( audiobuffer_t *b, const size_t legnth ) {
  b->l = legnth;
  b->a = calloc ( b->l, sizeof *b->a );
}


typedef struct {
  HWAVEOUT hwo;
  WAVEFORMATEX wfx;
  WAVEHDR header;
  audiobuffer_t *buffer;
} audio_t;


int
audio_stop ( audio_t *a ) {
  if ( a->buffer ) {
    waveOutReset ( a->hwo );
    waveOutUnprepareHeader ( a->hwo, &a->header, sizeof a->header );
    a->buffer = NULL;
  }
}


int
audio_kill ( audio_t *a ) {
  audio_stop ( a );
	waveOutClose ( a->hwo );
}


int
audio_init ( audio_t *a ) {
  a->hwo = NULL;
	a->wfx = ( WAVEFORMATEX ) { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
	waveOutOpen ( &a->hwo, WAVE_MAPPER, &a->wfx, 0, 0, CALLBACK_NULL);
  a->buffer = NULL;
}


int
audio_play ( audio_t *a, audiobuffer_t *b ) {
  audio_stop ( a );
  a->buffer = b;
  a->header = ( WAVEHDR ) { ( char * ) b->a, b->l, 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader ( a->hwo, &a->header, sizeof a->header );
  waveOutWrite ( a->hwo, &a->header, sizeof a->header );
}


#endif // AUDIO_H
