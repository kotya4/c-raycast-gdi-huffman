#ifndef HUSK_H
#define HUSK_H
#include "husktypes.h"//ht_clear,types
#include "huskdb.h"

void
husk_onpress
(char key
){
put_db(key);
}

void
husk_ondraw
(Tf f
,Tp p
,Td w
,Td h
){
ht_clear(p,w,h,0x111111);
flush_db(f,p,w,h);
draw_cursor(f,p,w,h);
}

void husk_init(){
create_db();
clear_db();
}
void husk_deinit(){
destroy_db();
}

#endif//HUSK_H
