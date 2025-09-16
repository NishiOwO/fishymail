/* $Id$ */

#include <fishymail.h>

int FishyMailMainRoutine(void) {
	int st;
	char* s = FishyMailGetVersion();
	DebugLog("FishyMail %s", s);
	free(s);

	if((st = FishyMailSocketInit()) != 0){
		return st;
	}

	return 0;
}

void FishyMailMainUIRoutine(void) {
	FishyMailPrepareUI();
	FishyMailShowMain();
}

void FishyMailMenuItemPressed(const char* str) {
	DebugLog("%s was pressed", str);
	if(strcmp(str, "MENU_FILE_QUIT") == 0) {
		FishyMailStartQuit();
	}
}
