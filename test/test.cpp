# include <stdio.h>


enum Beep { B_IDLE1 = 1, B_IDLE2 = 2 };
int score[2];
char gameMode;
char firstPlayer;

# include "ttc_selectIdleBeep.cpp"


int pass = 0;
int fail = 0;
int total = 0;


void check(char gm, int sc0, int sc1, int res) {
	
	gameMode = gm;
	score[0] = sc0;
	score[1] = sc1;

	int r = selectIdleBeep();

	printf(
		"%s GM=%d SC=%02d:%02d R=%d -> %d \n"
		,( r == res ? ". " : "F ")
		,gm,sc0,sc1,res,r
	);

	total++;
	if (r == res) {
		pass++;
	} else {
		fail++;
	}

} // checkfp()


void test21low() {

	check(21,0,0, B_IDLE1);
	check(21,1,0, B_IDLE1);
	check(21,0,1, B_IDLE1);
	check(21,4,0, B_IDLE1);
	check(21,0,4, B_IDLE1);
	check(21,5,0, B_IDLE2);
	check(21,3,2, B_IDLE2);

	check(21,5,4, B_IDLE2);
	check(21,6,4, B_IDLE1);
	check(21,14,0, B_IDLE1);

	check(21,15,0, B_IDLE2);
	check(21,16,0, B_IDLE2);

	check(21,15,5, B_IDLE1);
	check(21,15,10, B_IDLE2);
	check(21,15,15, B_IDLE1);
	check(21,20,15, B_IDLE2);
	check(21,20,20, B_IDLE1);

}


void test11low() {

	check(11,0,0, B_IDLE1);
	check(11,1,0, B_IDLE1);
	check(11,0,1, B_IDLE1);
	check(11,3,0, B_IDLE2);
	check(11,0,3, B_IDLE2);
	check(11,2,1, B_IDLE2);
	check(11,1,2, B_IDLE2);

	check(11,3,2, B_IDLE2);
	check(11,4,2, B_IDLE1);
	check(11,9,0, B_IDLE2);

	check(11,9,3, B_IDLE1);
	check(11,9,6, B_IDLE2);
	check(11,9,8, B_IDLE2);
	check(11,9,9, B_IDLE1);

}


void test21high() {

	check(21,20,20, B_IDLE1);
	check(21,21,20, B_IDLE2);
	check(21,21,21, B_IDLE1);
	check(21,22,21, B_IDLE2);
	check(21,22,22, B_IDLE1);

}


void test11high() {

	check(11,9,9, B_IDLE1);
	check(11,10,9, B_IDLE1);
	check(11,10,10, B_IDLE1);
	check(11,11,10, B_IDLE2);
	check(11,11,11, B_IDLE1);
	check(11,12,11, B_IDLE2);

}


int main() {

	test21low();
	test11low();
	test21high();
	test11high();

	printf(
		"--------------------------------\n"
		"   total=%d passed=%d failed=%d \n"
		,total,pass,fail
	);

	return 0;
}