# if ( defined(__unix__) || defined(__APPLE__) )
# include "posixino.hpp"
# else
# include <Arduino.h>
# endif

void setupTimerInterrupt();
