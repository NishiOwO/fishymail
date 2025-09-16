/* $Id$ */
#include <fishymail.h>

#ifdef _WIN32
void FishyMailDNSInit(void) {
}
#else
#include <resolv.h>

static struct __res_state statp;

int FishyMailDNSInit(void) {
	if(res_ninit(&statp) < 0) {
		return -1;
	}

#ifdef DEBUG
	statp.options |= RES_DEBUG;
#endif

	return 0;
}
#endif
