#ifndef DEBUG_H
#define DEBUG_H

#define ERR_HWND_IS_NULL -0x0001

#ifdef ERR_MSG

  #include <stdio.h> // printf
  #include <windows.h> // PostQuitMessage
  void
  err_msg ( int err ) {
    printf ( "( -0x%04x ) ", -err );
    if ( err == ERR_HWND_IS_NULL ) printf ( "ERR_HWND_IS_NULL\n" );
  }
  #define ERR( err )({             \
    if ( err < 0 ) {               \
      printf ( "%s: ", __func__ ); \
      err_msg ( err );             \
      PostQuitMessage ( err );     \
      return err;                  \
    }                              \
  })

#else

  #include <windows.h> // PostQuitMessage
  #define ERR( err )({             \
    if ( err < 0 ) {               \
      PostQuitMessage ( err );     \
      return err;                  \
    }                              \
  })
  
#endif // ERR_MSG


#endif // DEBUG_H