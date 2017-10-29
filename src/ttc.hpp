# if ( defined(__unix__) || defined(__APPLE__) )
# include "posixino.hpp"
# else
# include <Arduino.h>
# endif

// display 1
//
# define CLK1 12
# define DIO1 11

// display 2 
//
# define CLK2 10
# define DIO2 9

// beeper
//
# define BEEP 2

// pedals
//
# define PEDAL1 3
# define PEDAL2 4

void setupTimerInterrupt();
int pp(int n);
void beep(int mode);
void showResults();
void handlePedals();