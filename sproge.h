// Some example programs of sprog shading language v1.

#ifndef SPROGE_H
#define SPROGE_H


// vertical circle in red channel
// horisontal circle in green channel
// circle at cursor in blue channel
// a1/ is radius = 0.1
#define SPROGE_CIRCLE_DANCE     \
  "a1/ 2x21/-^   2y13t%--^ +<", \
  "a1/ 2x13t%--^ 2y21/-^   +<", \
  "a1/ 2xX-^     2yY-^     +<"


// see red color if mouse on left side of window
// and yellow on right
#define SPROGE_MOUSE_SPLIT \
  "1",                     \
  "X21/<2*1-@",            \
  "0"


// animated xor.. thing.
#define SPROGE_XOR_DIVISION \
  "1t+ff*/ff*x*ff*y*~/",    \
  "0",                      \
  "0"


// channel 0 duplicates channel 1
// channel 1 duplicates channel 2
// channel 2 multiplies channel 3 and 4 and makes 0 every
//           pixel except rect ( 0.4, 0.4, 0.6, 0.6 )
// channel 3 creates rect ( 0, 0, x+0.1, y+0.1 )
// channel 4 creates rect ( x-0.1, y-0.1, 1, 1 )
// channel 5 is x = 0.5
// channel 6 is y = 0.5
#define SPROGE_WHITE_RECT    \
  "1@",                      \
  "1@",                      \
  "1@2@*",                   \
  "  a1/  2@<   a1/  3@< *", \
  "1@a1/10-*< 2@a1/10-*< *", \
  "21/x-",                   \
  "21/y-"


// modification of SPROGE_WHITE_RECT,
// channel 0 to 2 have their own rect
// functions with changing sizes,
// althrough this program can
// be golfscripted further.
#define SPROGE_PULSE_RECT  \
  "3@4@*",                 \
  "4@5@*",                 \
  "5@6@*",                 \
  "  8@  6@<   b@  7@< *", \
  "5@7@10-*< 6@a@10-*< *", \
  "  8@  4@<   6@  5@< *", \
  "3@7@10-*< 4@5@10-*< *", \
  "  6@  2@<   7@  3@< *", \
  "1@5@10-*< 2@6@10-*< *", \
  "21/x-",                 \
  "21/y-",                 \
  "a1/",                   \
  "a1/",                   \
  "a1/a1/a1/2*ft/%-A+",    \
  "a1/a1/a1/2*ft/%-A+"


#endif // SPROGE_H