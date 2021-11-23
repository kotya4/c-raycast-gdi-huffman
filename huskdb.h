#ifndef HUSKDB_H
#define HUSKDB_H
#include "husktypes.h"//all types
#include "font.h"//font_paint

#define dbw 12
#define dbh 5
char*db;//textbuffer
int dbx=0;
int dby=0;
void
create_db(){
db=malloc(dbw*dbh*sizeof*db);
}
void
destroy_db(){
free(db);db=NULL;
}
void
flush_db(Tf f,Tp p,Td w,Td h){
//font_paint(f,p,w,h,0,0,"test",0xffffff);
//return;
const int fh=f->ch;
int i=0;
for(int y=0;y<dbh;++y){
	char b[dbw];
	int x;
	for(x=0;x<dbw;++x){
		b[x]=db[i];
		i++;
	}
	b[x]=0;
	font_paint(f,p,w,h,0,y*fh,b,0xffffff);//no color buffer for now
//TODO: dristnya
}
}
void
clear_db(){
for(int i=0;i<dbw*dbh;++i)db[i]=0;
//db[0]='H';
//db[dbw]='E';
//db[dbw*2]='L';
//db[dbw*3]='L';
//db[dbw*4]='O';
}
void
endl_db(){
dbx=0;
dby++;
if(dby>=dbh){
dby=0;
clear_db();
}
}
void
put_db(char v){
if(v==13){
endl_db();
}else if(v==27){//backslash code? escape for now
//TODO
}else{
const int i=dbx+dby*dbw;
db[i]=v;
dbx++;
if(dbx>=dbw){
endl_db();
}
}
}
void
draw_cursor(Tf f,Tp p,Td w,Td h){
font_paint(f,p,w,h,dbx*f->cw,dby*f->ch,"■\0",0xa00000);
}
#endif//HUSKDB_H
