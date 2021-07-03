// prints warnings in debug mode
#ifndef WARN_H
#define WARN_H

#ifdef WARNMEPLS

  #include <stdio.h> // printf

  #define WARN( ... ) ({           \
    printf ( "%s:: ", __func__ );  \
    printf ( __VA_ARGS__  );       \
  })

#else

  #define WARN( ... ) ({})

#endif // WARNMEPLS

#endif // WARN_H
