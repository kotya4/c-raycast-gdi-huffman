#ifndef HUSK_H
#define HUSK_H
#include "font.h"//font_t,font_paint

char input[128];
int inputlen=0;
int cx=0;
int cy=0;

void
husk_onpress
(char key
){
input[inputlen]=key;
inputlen=(inputlen+1)&127;
}

typedef font_t * f_t;
typedef unsigned char * p_t;//pixelbuffer
typedef unsigned int pd_t;//pixelbuffer dimention type
typedef unsigned int color_t;

void
husk_ondraw
(const f_t f
,p_t p
,pd_t w
,pd_t h
){
font_paint(f,p,w,h,cx,cy,input,0xff0000);
}


#endif//HUSK_H
