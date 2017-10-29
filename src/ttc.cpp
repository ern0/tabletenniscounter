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
	int counter = 0;
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

		display1.setBrightness(0x07);
		display1.setSegments(welcome);



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

		delay(1000);
		Serial.println(counter);

	} // loop
