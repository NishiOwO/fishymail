/* $Id$ */

#include <fishymail.h>

void FishyMailSanitizeName(const char* from, char* to) {
	int i;
	int ind = 0;
	for(i = 0; from[i] != 0; i++) {
		if('a' <= from[i] && from[i] <= 'z') {
			to[ind++] = toupper((int)from[i]);
		} else if('A' <= from[i] && from[i] <= 'Z') {
			to[ind++] = from[i];
		} else if('0' <= from[i] && from[i] <= '9') {
			to[ind++] = from[i];
		} else if(from[i] == '_' || from[i] == ' ') {
			to[ind++] = from[i];
		}
		to[ind + 1] = 0;
	}
}
