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

	const uint8_t game21Segments[] = {
		SEG_A | SEG_F | SEG_E | SEG_D | SEG_C,
		SEG_G,
		SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
		SEG_B | SEG_C
	};

	const uint8_t game11Segments[] = {
		SEG_A | SEG_F | SEG_E | SEG_D | SEG_C,
		SEG_G,
		SEG_B | SEG_C,
		SEG_B | SEG_C
	};

	const uint8_t darkSegments[] = { 0,0,0,0 };


	TM1637Display display1(CLK1,DIO1);
	TM1637Display display2(CLK2,DIO2);

	TM1637Display* display[2] = { 
		&display1,
		&display2
	};

	char gameMode;
	char gameSelector;
	char matchOver;


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

		for (int n = 0; n < 2; n++) {

			pinMode(pepin(n),INPUT_PULLUP);

			tick[n] = 1;
			score[n] = 0;
			brite[n] = HALFBRITE;

			pressConfirm[n] = 0;
			lastClick[n] = 0;
			clickCount[n] = 0;
			lastState[n] = false;

			event[n] = E_IDLE;

		}

		beep(B_WELCOME);

		for (int v = HALFBRITE; v <= FULLBRITE; v++) {
			for (int n = 0; n < 2; n++) setBrightness(n,v);
			setSegments(welcomeSegments);
			delay(200);
		}

		gameMode = 21;
		gameSelector = 21;
		matchOver = M_FIRST;

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
		matchOverAnim();
		delay(1);

	} // loop


	void setSegments(const uint8_t* segments) {

		for (int n = 0; n < 2; n++) {
			display[n]->setSegments(segments);
		}

	} // setSegments()


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
					event[n] = E_HOLD;
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
							if (matchOver == M_PLAYING) tick[n] = 1;
						}

					} else {

						if (matchOver == M_PLAYING) tick[n] = 2;

					}

				} // else idle

			} // else not press

			lastState[n] = prec;

		} // for sides
	} // handleClicks()


	void setBrightness(int n,int v) {
		if (matchOver != M_PLAYING) return;

		brite[n] = v;

		if (brite[1 - n] == HALFBRITE) v = HALFBRITE;

		for (int n = 0; n < 2; n++) {
			display[n]->setBrightness(v);
		}
	} // setBrightness()
	

	void beepGameStart(int p) {

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < p; j++) bip(5,100);
			bip(100,400);		
		}

	} // beepGameStart()


	void bip(int a,int b) {
	
		digitalWrite(BEEP,HIGH);
		delay(a);
		digitalWrite(BEEP,LOW);
		if (b == 0) return;
		delay(b);
	
	} // bip()


	void beep(int mode) {

		switch (mode) {

		case B_WELCOME:
			bip(100,0);
			break;

		case B_IDLE2:
			bip(5,100);
			// fall to idle1

		case B_IDLE1:
			bip(5,0);
			break;

		case B_G21:
			beepGameStart(2);
			break;

		case B_G11:
			beepGameStart(1);
			break;

		case B_SERVECHANGE:
			for (int i = 0; i < 3; i++) bip(5,100);
			break;

		case B_VICTORY:
			bip(30,400);
			bip(5,200);
			bip(5,200);
			bip(30,400);
			bip(5,200);
			bip(5,200);
			bip(30,400);
			bip(30,400);
			bip(80,400);
			break;

		} // switch

	} // beep()


	int rz(int v) {
		return ( v < 0 ? 0 : v );
	} // rz()


	void showResults() {

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
			if (event[n] == E_GAMESTART) procGameStart(n);

		} // for

	} // procEvent()


	void procClick(int n) {

		if (matchOver != M_PLAYING) {
			event[n] = E_NONE;
			return;
		}

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
			setBrightness(n,FULLBRITE);
			showResults();
			
			return;

		} // switch

		event[n] = E_NONE;
		setBrightness(n,HALFBRITE);
		showResults();

	} // procClick()


	void procHold(int n) {

		int hold = tick[n] - lastClick[n];
		if (lastClick[n] == 0) return;

		if (hold < T_HOLD) {
			gameSelector = 0;
			event[n] = E_NONE;
			return;
		}

		matchOver = M_PLAYING;
		setBrightness(n,FULLBRITE);

		if (clickCount[n] == 1) {
			gameSelector = 21;
			setSegments(game21Segments);
		} else {
			gameSelector = 11;
			setSegments(game11Segments);
		}
		
		event[n] = E_GAMESTART;

	} // procHold()


	void procIdle(int n) {		
	
		if (matchOver != M_PLAYING) {
			event[n] = E_NONE;
			return;
		}

		setBrightness(n,FULLBRITE);

		for (int n = 0; n < 2; n++) {
			if (score[n] < 0) score[n] = 0;
		}

		showResults();
		procMatchOver();

		if (matchOver == M_PLAYING) {
	
			if (serveChange()) {
				beep(B_SERVECHANGE);
				delay(600);
			}

			beep( selectIdleBeep() );

		} // if not match over

		event[n] = E_NONE;

	} // procIdle()


	void procGameStart(int n) {

		delay(600);

		if (gameSelector == 21) {
			setSegments(game21Segments);
			beep(B_G21);
		}

		if (gameSelector == 11) {
			setSegments(game11Segments);
			beep(B_G11);
		}

		matchOver = M_PLAYING;
		gameMode = gameSelector;
		delay(1200);

		for (int n = 0; n < 2; n++) score[n] = 0;
		showResults();

		event[n] = E_NONE;

	} // procGameStart()


	bool serveChange() {

		int total = score[0] + score[1];
		
		if (total == 0) return false;
		if (total > (gameMode == 21 ? 40 : 20)) return true;		

		return ( total % (gameMode == 21 ? 5 : 3) == 0);
	} // serveChange()


	void procMatchOver() {
		if (matchOver != M_PLAYING) return;

		if ( (score[0] < gameMode) && (score[1] < gameMode) ) return;

		int delta = score[0] - score[1];
		if (delta < 0) delta = -delta;

		if (delta < 2) return;

		matchOver = M_OVER;
		delay(600);
		beep(B_VICTORY);
		tick[0] = 0;

	} // procMatchOver()


	void matchOverAnim() {
		if (matchOver == M_PLAYING) return;

		if (tick[0] % 16 > 11) {
			setSegments(darkSegments);
		} else {
			if (matchOver == M_OVER) {
				showResults();
			} else {
				setSegments(welcomeSegments);
			}

		}

	} // matchOverAnim()


	char selectIdleBeep() {

		int sc = score[0] + score[1];
		int swap;

		if (sc <= ( gameMode == 21 ? 40 : 20 )) {
			swap = sc / ( gameMode == 21 ? 5 : 3 );
		} else {
			swap = sc;
		}

		if (swap % 2 == 0) {
			return B_IDLE1;
		} else {
			return B_IDLE2;
		}
			
	} // selectIdleBeep()