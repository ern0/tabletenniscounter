#include "ttc.hpp"

#ifndef POSIXINO
#include <TM1637Display.h>
#endif

#define SEG_DOT 0x80

	//      A
	//     ---
	//  F |   | B    *
	//     -G-      DOT
	//  E |   | C    *
	//     ---
	//      D

	const uint8_t welcome[] = {
		SEG_G | SEG_E | SEG_D,
		SEG_E | SEG_G | SEG_C,
		SEG_F | SEG_G | SEG_E | SEG_D,
		SEG_G | SEG_E
	};

	TM1637Display display1(CLK1,DIO1);
	TM1637Display display2(CLK2,DIO2);

	TM1637Display* display[2] = { 
		&display1,
		&display2
	};

	bool changed;

	int tick[2];
	int score[2];

	int pressConfirm[2];
	int lastClick[2];
	int clickCount[2];
	EventType event[2];
	bool lastState[2];


	void setup() {

		Serial.begin(38400);
		Serial.println("table tennis counter");

		setupTimerInterrupt();
		pinMode(BEEP,OUTPUT);

		changed = true;

		for (int n = 0; n < 2; n++) {

			pinMode(pepin(n),INPUT_PULLUP);

			tick[n] = 0;
			score[n] = 0;

			pressConfirm[n] = 0;
			lastClick[n] = 0;
			clickCount[n] = 0;
			event[n] = E_NONE;
			lastState[n] = false;

			display[n]->setBrightness(0x07);
			display[n]->setSegments(welcome);
			display[n]->setSegments(welcome);
		}

		beep(BEEP_WELCOME);
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

		for (int n = 0; n < 2; n++) tick[n]++;

	} // timer1 interrupt


	void loop() {

		handleClicks();
		calcResults();
		showResults();
		delay(1);

	} // loop


	int pepin(int n) {
		return ( n == 0 ? PEDAL1 : PEDAL2 );
	}


	void beep(int mode) {

return; ///
		digitalWrite(BEEP,HIGH);
		delay(20);
		digitalWrite(BEEP,LOW);

	} // beep()


	void showResults() {

		if (!changed) return;
		changed = false;
		
		for (int n = 0; n < 2; n++) {
			display[n]->showNumberDecEx(score[n],0xff,true,2,0);
			display[n]->showNumberDecEx(score[1 - n],0,true,2,2);
		}

	} // showResults()


	void calcResults() {
		for (int n = 0; n < 2; n++) {

			switch (event[n]) {

			case E_CLICK:
				score[n]++;
				changed = true;
				break;

			case E_DOUBLECLICK:
				score[n]--;
				score[1 - n]++;
				changed = true;
				break;

			case E_TRIPLECLICK:
				score[1 - n]--;
				changed = true;
				break;

			case E_NONE:
			case E_IDLE:
				// nop
				break;
			
			} // switch

			event[n] = E_NONE;

		} // for sides
	} // calcResults()


	void handleClicks() {
		for (int n = 0; n < 2; n++) {

			// avoid prelling, confirm press
			bool press = !digitalRead(pepin(n));
			if (press) {
				pressConfirm[n]++;
			} else {
				pressConfirm[n] = 0;
			}
			bool prec = (pressConfirm[n] > 2);

			if (prec) {

				if (lastState[n]) {  // press -> press: hold
					// nop
				}  // if hold
			
				else {  // off -> press: click

					if (lastClick[n] != 0) {
						if (tick[n] - lastClick[n] > T_CLICKCLICK) {
							lastClick[n] = 0;
							clickCount[n] = 0;
						}
					} // if out of multi-click window

					if (lastClick[n] == 0) clickCount[n] = 0;
					clickCount[n]++;

					if (clickCount[n] > 3) clickCount[n] -= 3;
					if (clickCount[n] == 1) event[n] = E_CLICK;
					if (clickCount[n] == 2) event[n] = E_DOUBLECLICK;
					if (clickCount[n] == 3) event[n] = E_TRIPLECLICK;

					lastClick[n] = tick[n];

				} // else click

			} // if press

			else {  // not press

				if (!lastState[n]) {  // press -> off: release
					// nop
				} // if release

				else {  // off -> off: idle
					if (lastClick[n] > 0) {
						if (tick[n] - lastClick[n] > T_CLICKCLICK) {
							lastClick[n] = 0;
							event[n] = E_IDLE;
						}
					}
				} // else idle

			} // else not press

			lastState[n] = prec;

		} // for sides
	} // handleClicks()

