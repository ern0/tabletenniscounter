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
