#ifndef HUSKTYPES_H
#define HUSKTYPES_H
#include "font.h"//font_t

typedef font_t * Tf;
typedef unsigned char * Tp;//pixelbuffer
typedef unsigned int Td;//pixelbuffer dimention type
typedef unsigned int Tc;

void ht_clear(Tp,Td,Td,Tc);

void
ht_clear
(Tp p
,Td w
,Td h
,Tc c
){
for(int y=0;y<h;++y)for(int x=0;x<w;++x){
const int i=(w*y+x)*3;
p[i+0]=(c>>16)&0xff;
p[i+1]=(c>> 8)&0xff;
p[i+2]=(c>> 0)&0xff;
}
}

#endif//HUSKTYPES_H
