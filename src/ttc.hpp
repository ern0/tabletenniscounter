# if ( defined(__unix__) || defined(__APPLE__) )
# include "posixino.hpp"
# else
# include <Arduino.h>
# endif

// pins
# define CLK1 12
# define DIO1 11
# define CLK2 10
# define DIO2 9
# define BEEP 2
# define PEDAL1 3
# define PEDAL2 4

enum EventType {
	E_NONE, E_IDLE, E_CLICK, E_HOLD
};

enum BeepType {
	BEEP_WELCOME
};

// time values (1/10 sec)
# define T_CLICKCLICK 10
# define T_LONGPRESS 40

void setupTimerInterrupt();
inline int pepin(int n);
void handleClicks();
void beep(int mode);
void showResults();
void calcResults();
void procClick(int n);
void procHold(int n);
void procIdle(int n);