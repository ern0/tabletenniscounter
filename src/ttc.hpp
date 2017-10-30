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
	B_WELCOME, B_IDLE
};

// time values (1/10 sec)
# define T_CLICKCLICK 8
# define T_LONGPRESS 40

// brightness values
# define HALFBRITE 8
# define FULLBRITE 15


void setupTimerInterrupt();
void welcome();
inline int pepin(int n);
void handleClicks();
void setBrightness(int n,int v);
void beep(int mode);
inline int rz(int v);
void showResults();
void procEvent();
void procClick(int n);
void procHold(int n);
void procIdle(int n);