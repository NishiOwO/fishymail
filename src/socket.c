/* $Id$ */
#include <fishymail.h>

#ifdef USE_CYRUS_SASL2
#include <sasl/sasl.h>
#endif

#ifdef USE_SSL
#include <openssl/ssl.h>
#endif

#ifdef _WIN32
#include <winsock.h>
#endif

void FishyMailSocketInit(void) {
#ifdef _WIN32
	{
		WSADATA wsa;
		WSAStartup(MAKEWORD(1, 1), &wsa);
		DebugLog("WinSock initialized, version %d.%d", LOBYTE(wsa.wVersion), HIBYTE(wsa.wVersion));
	}
#endif
#ifdef USE_CYRUS_SASL2
	{
		DebugLog("Cyrus SASL %d.%d.%d", SASL_VERSION_MAJOR, SASL_VERSION_MINOR, SASL_VERSION_STEP);
	}
#endif
#ifdef USE_SSL
	{
		SSL_library_init();
		DebugLog("%s", OPENSSL_VERSION_TEXT);
	}
#endif
}
