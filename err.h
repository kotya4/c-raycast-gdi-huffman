#ifndef ERR_H
#define ERR_H

#define ERR_HWND_IS_NULL  -0x0001
#define ERR_SPROG_FAILED  -0x0002

#include <stdlib.h> // exit

#ifdef ERR_MSG

  #include <stdio.h> // printf

  void
  err_msg ( int err ) {
    printf ( "( -0x%04x ) ", -err );
    if ( err == ERR_HWND_IS_NULL  ) printf ( "ERR_HWND_IS_NULL\n" );
    if ( err == ERR_SPROG_FAILED  ) printf ( "ERR_SPROG_FAILED\n" );
  }

  #define ERR( err ) ({            \
    if ( err < 0 ) {               \
      printf ( "%s: ", __func__ ); \
      err_msg ( err );             \
      exit ( err );                \
    }                              \
  })

  #define WARN( ... ) ({           \
    printf ( "%s: ", __func__ );   \
    printf ( __VA_ARGS__  );       \
  })

#else // ERR_MSG

  #define ERR( err )({             \
    if ( err < 0 ) {               \
      exit ( err );                \
    }                              \
  })

  #define WARN( ... ) ({})

#endif // ERR_MSG


#endif // ERR_H