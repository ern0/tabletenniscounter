#include "ttc.hpp"

#ifndef POSIXINO
#include <TM1637Display.h>
#endif

#define SEG_DOT 0x80

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D

	TM1637Display display1(CLK1,DIO1);
	TM1637Display display2(CLK2,DIO2);

	TM1637Display* display[2] = { 
		&display1,
		&display2
	};

	int score[2];
	int counter;

	char welcome[] = {

		SEG_G | SEG_E | SEG_D,
		SEG_E | SEG_G | SEG_C | SEG_DOT,
		SEG_F | SEG_G | SEG_E | SEG_D,
		SEG_G | SEG_E

	};


	void setup() {

		Serial.begin(38400);
		Serial.println("table tennis counter");

		setupTimerInterrupt();
		pinMode(BEEP,OUTPUT);

		counter = 0;

		for (int n = 0; n < 2; n++) {

			pinMode(pp(n),INPUT_PULLUP);

			score[n] = 0;

			display[n]->setBrightness(0x07);
			display[n]->setSegments(welcome);
			display[n]->setSegments(welcome);
		}

		beep(1);
		delay(800);

	} // setup()


	void setupTimerInterrupt() {

		cli();

		TCCR1A = 0; // set entire TCCR1A register to 0
		TCCR1B = 0; // same for TCCR1B
		TCNT1  = 0; // initialize counter value to 0

		// set compare match register 15624 = 1 Hz
		OCR1A = 15624 / 10;
		// turn on CTC mode
		TCCR1B |= (1 << WGM12);
		// Set CS12 and CS10 bits for 1024 prescaler
		TCCR1B |= (1 << CS12) | (1 << CS10);
		// enable timer compare interrupt
		TIMSK1 |= (1 << OCIE1A);

		sei();

	} // setupTimerInterrupt()


	ISR(TIMER1_COMPA_vect) {

		counter++;

	} // timer1 interrupt


	void loop() {

		handlePedals();
		showResults();

		delay(100);

	} // loop


	int pp(int n) {
		return ( n == 0 ? PEDAL1 : PEDAL2 );
	}


	void beep(int mode) {

		digitalWrite(BEEP,HIGH);
		delay(200);
		digitalWrite(BEEP,LOW);

	} // beep()


	void showResults() {

		// args: value, dot bitmask, leading zero, length, position
		
		for (int n = 0; n < 2; n++) {
			display[n]->showNumberDecEx(score[n],0xff,true,2,0);
			display[n]->showNumberDecEx(score[1 - n],0,true,2,2);
		}

	} // showResults()


	void handlePedals() {

		for (int n = 0; n < 2; n++) {
			int press = !digitalRead(pp(n));
			if (press) {
				score[n]++;
			}

		}

	} // handlePedals()