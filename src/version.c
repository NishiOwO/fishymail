/* $Id$ */
#include <fishymail.h>

const char* FishyMailVersion = "1.0.0";

char* FishyMailGetVersion(void){
	char* ver = malloc(512);

	strcpy(ver, FishyMailVersion);
#ifdef USE_CYRUS_SASL2
	strcat(ver, "+sasl");
#endif
#ifdef USE_SSL
	strcat(ver, "+ssl");
#endif

	return ver;
}
