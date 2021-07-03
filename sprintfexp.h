// inline sprintf expantion
#ifndef SPRINTFEXP_H
#define SPRINTFEXP_H
#include <stdio.h>

char sprintf128_buffer[ 128 ];
#define sprintf128( ... ) ({                        \
  snprintf ( sprintf128_buffer, 128, __VA_ARGS__ ); \
  sprintf128_buffer;                                \
})

#endif // SPRINTFEXP_H