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

	const uint8_t welcomeSegments[] = {
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
	int brite[2];

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
			brite[n] = HALFBRITE;

			pressConfirm[n] = 0;
			lastClick[n] = 0;
			clickCount[n] = 0;
			event[n] = E_NONE;
			lastState[n] = false;

		}

		beep(B_WELCOME);

		for (int v = HALFBRITE; v <= FULLBRITE; v++) {
			for (int n = 0; n < 2; n++) setBrightness(n,v);
			welcome();
			delay(200);
		}

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
		procEvent();
		delay(1);

	} // loop


	void welcome() {
		for (int n = 0; n < 2; n++) {
			display[n]->setSegments(welcomeSegments);
		}
	} // welcome()


	int pepin(int n) {
		return ( n == 0 ? PEDAL1 : PEDAL2 );
	}


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

					if (clickCount[n] > 6) clickCount[n] = 6;
					event[n] = E_CLICK;

					lastClick[n] = tick[n];

				} // else click

			} // if press

			else {  // not press

				if (lastState[n]) {  // press -> off: release
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


	void setBrightness(int n,int v) {

		brite[n] = v;

		if (brite[1 - n] == HALFBRITE) v = HALFBRITE;

		for (int n = 0; n < 2; n++) {
			display[n]->setBrightness(v);
		}
	} // setBrightness()
	

	void beep(int mode) {

		switch (mode) {

		case B_WELCOME:
			digitalWrite(BEEP,HIGH);
			delay(20);
			digitalWrite(BEEP,LOW);
			break;

		case B_IDLE:
			digitalWrite(BEEP,HIGH);
			delay(5);
			digitalWrite(BEEP,LOW);
			break;

		} // switch

	} // beep()


	int rz(int v) {
		return ( v < 0 ? 0 : v );
	} // rz()


	void showResults() {

		if (!changed) return;
		changed = false;
		
		for (int n = 0; n < 2; n++) {
			display[n]->showNumberDecEx(rz(score[n]),0xff,true,2,0);
			display[n]->showNumberDecEx(rz(score[1 - n]),0,true,2,2);
		}

	} // showResults()


	void procEvent() {

		for (int n = 0; n < 2; n++) {

			if (event[n] == E_CLICK) procClick(n);
			if (event[n] == E_HOLD) procHold(n);
			if (event[n] == E_IDLE) procIdle(n);

		} // for

		showResults();

	} // procEvent()


	void procClick(int n) {

		switch (clickCount[n]) {

		case 1:
			score[n]++;
			break;

		case 2:
			--score[n];
			score[1 - n]++;
			break;

		case 3:
			--score[1 - n];
			--score[n];
			break;

		case 4:
			score[n]++;
			--score[1 - n];
			break;

		case 5:
			score[1 - n]++;
			clickCount[n] = 0;
			lastClick[n] = 0;
			event[n] = E_IDLE;
			return;

		} // switch

		event[n] = E_NONE;

		setBrightness(n,HALFBRITE);
		changed = true;

	} // procClick()


	void procHold(int n) {

		// TODO

		event[n] = E_NONE;

	} // procHold()


	void procIdle(int n) {

		for (int n = 0; n < 2; n++) {
			if (score[n] < 0) score[n] = 0;
		}

		beep(B_IDLE);
		setBrightness(n,FULLBRITE);
		changed = true;

		event[n] = E_NONE;

	} // procIdle()


