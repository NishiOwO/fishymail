/* $Id$ */

#include <fishymail.h>

// use this instead of isalnum to avoid depending on the current locale
static inline int is_alnum(char c) {
	return ('a' <= c && c <= 'z') ||
	       ('A' <= c && c <= 'Z') ||
	       ('0' <= c && c <= '9');
}

void FishyMailRemoveSpecial(const char* from, char* to) {
	int i;
	int ind = 0;
	for(i = 0; from[i] != 0; i++) {
		if(is_alnum(from[i]) || from[i] == '_' || from[i] == ' ') {
			to[ind++] = from[i];
		}
	}
	to[ind] = 0;
}

void FishyMailSanitizeName(const char* from, char* to) {
	int i;
	int ind = 0;
	for(i = 0; from[i] != 0; i++) {
		if(is_alnum(from[i]) || from[i] == '_' || from[i] == ' ') {
			unsigned char c = (unsigned char)from[i];
			to[ind++]	= (char)toupper(c);
		}
	}
	to[ind] = 0;
}
