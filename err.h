#ifndef ERR_H
#define ERR_H

#define ERR_HWND_IS_NULL -0x0001

#ifdef ERR_MSG
  
  #include <stdlib.h> // exit
  #include <stdio.h> // printf
  void
  err_msg ( int err ) {
    printf ( "( -0x%04x ) ", -err );
    if ( err == ERR_HWND_IS_NULL ) printf ( "ERR_HWND_IS_NULL\n" );
  }
  #define ERR( err )({             \
    if ( err < 0 ) {               \
      printf ( "%s: ", __func__ ); \
      err_msg ( err );             \
      exit ( err );                \
    }                              \
  })

#else
  
  #include <stdlib.h> // exit
  #define ERR( err )({             \
    if ( err < 0 ) {               \
      exit ( err );                \
    }                              \
  })
  
#endif // ERR_MSG


#endif // ERR_H