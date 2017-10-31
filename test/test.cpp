# include <stdio.h>


enum Beep { B_IDLE1 = 1, B_IDLE2 = 2 };
int score[2];
char gameMode;
char firstPlayer;

# include "ttc_selectIdleBeep.cpp"


int pass = 0;
int fail = 0;
int total = 0;


void checkfp(char gm, int fp, int sc0, int sc1, int res) {
	
	gameMode = gm;
	score[0] = sc0;
	score[1] = sc1;
	firstPlayer = fp;

	int r = selectIdleBeep();

	printf(
		"%s GM=%d FP=%d SC=%02d:%02d R=%d -> %d \n"
		,( r == res ? ". " : "F ")
		,gm,fp,sc0,sc1,res,r
	);

	total++;
	if (r == res) {
		pass++;
	} else {
		fail++;
	}

} // checkfp()


void check(char gm, int sc0, int sc1, int res) {
	checkfp(gm,0,sc0,sc1,res);
	checkfp(gm,1,sc0,sc1,( res == B_IDLE1 ? B_IDLE2 : B_IDLE1 ));
} // check()


int main() {

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

	printf("   total=%d passed=%d failed=%d \n",total,pass,fail);
	return 0;
}