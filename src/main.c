/* $Id$ */

#include <fishymail.h>

void FishyMailMainRoutine(void) {}

void FishyMailMainUIRoutine(void) {
	char* s = FishyMailGetVersion();
	DebugLog("FishyMail %s", s);
	free(s);

	FishyMailPrepareUI();
	FishyMailShowMain();
}

void FishyMailMenuItemPressed(const char* str) {
	DebugLog("%s was pressed", str);
	if(strcmp(str, "MENU_FILE_QUIT") == 0) {
		FishyMailStartQuit();
	}
}
