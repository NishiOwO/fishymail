/* $Id$ */
#include <fishymail.h>

IMAP_HANDLE FishyMailConnectIMAP(const char* host, int port, int ssl){
	FishyMailConnect(host, port, ssl);

	return NULL;
}
