/* $Id$ */
#include <fishymail.h>

#include <netincl.h>

typedef struct socket {
	int fd;
#ifdef USE_SSL
	SSL_CTX* ctx;
	SSL*	 ssl;
#endif
} socket_t;

int FishyMailSocketInit(void) {
#ifdef _WIN32
	{
		WSADATA wsa;
		WSAStartup(MAKEWORD(2, 0), &wsa);
		DebugLog("WinSock initialized, version %d.%d", LOBYTE(wsa.wVersion), HIBYTE(wsa.wVersion));
	}
#endif
#ifdef USE_CYRUS_SASL2
	{
		if(sasl_client_init(NULL) != SASL_OK) {
			DebugLog("SASL failed");
			return -1;
		}
		DebugLog("Cyrus SASL %d.%d.%d", SASL_VERSION_MAJOR, SASL_VERSION_MINOR, SASL_VERSION_STEP);
	}
#endif
#ifdef USE_SSL
	{
		SSL_library_init();
		DebugLog("%s", OPENSSL_VERSION_TEXT);
	}
#endif
	return 0;
}

SOCKET_HANDLE FishyMailConnect(const char* host, int port, int ssl) {
	socket_t*	     s = malloc(sizeof(*s));
	FishyMailDNSPacket_t pkt;
	int		     id;
	int		     i;
	int		     sockok = 0;

	(void)ssl;

	for(i = 0; i < 2; i++) {
		int j;

		if(i == 0) id = DNSPACKET_A;
		if(i == 1) id = DNSPACKET_AAAA;

		FishyMailDNSLookup(&pkt, host, id);
		for(j = 0; j < pkt.count; j++) {
			int fd = socket(id == DNSPACKET_A ? PF_INET : PF_INET6, SOCK_STREAM, 0);
			if(fd == -1) break;
			if(id == DNSPACKET_A) {
				struct sockaddr_in in;

				in.sin_family = AF_INET;
				in.sin_port   = htons(port);
				in.sin_addr   = *(struct in_addr*)pkt.result[j];

				if(connect(fd, (struct sockaddr*)&in, sizeof(in)) == 0) {
					sockok = 1;
					break;
				} else {
					net_close(fd);
				}
			} else if(id == DNSPACKET_AAAA) {
				struct sockaddr_in6 in6;

				in6.sin6_family = AF_INET6;
				in6.sin6_port	= htons(port);
				in6.sin6_addr	= *(struct in6_addr*)pkt.result[j];

				if(connect(fd, (struct sockaddr*)&in6, sizeof(in6)) == 0) {
					sockok = 1;
					break;
				} else {
					net_close(fd);
				}
			}
		}
		FishyMailFreeDNSPacket(&pkt);

		if(sockok) break;
	}

	if(!sockok) {
		DebugLog("%s:%d: connection failure", host, port);
		free(s);
		return NULL;
	}

	return (SOCKET_HANDLE)s;
}
