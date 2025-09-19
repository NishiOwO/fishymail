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

unsigned long FishyMailRead(SOCKET_HANDLE handle, void* data, unsigned long len){
	unsigned long code;
	socket_t* s = (socket_t*)handle;
#ifdef USE_SSL
	if(s->ssl != NULL){
		code = SSL_read(s->ssl, data, len);
	}else{
		code = net_read(s->fd, data, len);
	}
#else
	code = net_read(s->fd, data, len);
#endif
	return code;
}

unsigned long FishyMailWrite(SOCKET_HANDLE handle, void* data, unsigned long len){
	unsigned long code;
	socket_t* s = (socket_t*)handle;
#ifdef USE_SSL
	if(s->ssl != NULL){
		code = SSL_write(s->ssl, data, len);
	}else{
		code = net_write(s->fd, data, len);
	}
#else
	code = net_write(s->fd, data, len);
#endif
	return code;
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
			int nbyt;
			int yes = 1;

			s->fd = socket(id == DNSPACKET_A ? PF_INET : PF_INET6, SOCK_STREAM, 0);
			if(s->fd == -1) break;

			nbyt = 65535; setsockopt(s->fd, SOL_SOCKET, SO_RCVBUF, (char*)&nbyt, sizeof(nbyt));
			nbyt = 65535; setsockopt(s->fd, SOL_SOCKET, SO_SNDBUF, (char*)&nbyt, sizeof(nbyt));
			setsockopt(s->fd, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes));

			if(id == DNSPACKET_A) {
				struct sockaddr_in in;

				in.sin_family = AF_INET;
				in.sin_port   = htons(port);
				in.sin_addr   = *(struct in_addr*)pkt.result[j];

				if(connect(s->fd, (struct sockaddr*)&in, sizeof(in)) == 0) {
					sockok = 1;
					break;
				} else {
					net_close(s->fd);
				}
			} else if(id == DNSPACKET_AAAA) {
				struct sockaddr_in6 in6;

				in6.sin6_family = AF_INET6;
				in6.sin6_port	= htons(port);
				in6.sin6_addr	= *(struct in6_addr*)pkt.result[j];

				if(connect(s->fd, (struct sockaddr*)&in6, sizeof(in6)) == 0) {
					sockok = 1;
					break;
				} else {
					net_close(s->fd);
				}
			}
		}
		FishyMailFreeDNSPacket(&pkt);

		if(sockok) break;
	}

#ifdef USE_SSL
	if(ssl) {
		const SSL_METHOD* method = TLS_client_method();
		s->ctx = SSL_CTX_new(method);
		s->ssl = SSL_new(s->ctx);
		SSL_set_fd(s->ssl, s->fd);
		SSL_set_tlsext_host_name(s->ssl, host);
		if(SSL_connect(s->ssl) != 1){
			SSL_CTX_free(s->ctx);
			SSL_free(s->ssl);
			net_close(s->fd);
			DebugLog("SSL connection failure");
			free(s);
			return NULL;
		}
	}else{
		s->ctx = NULL;
		s->ssl = NULL;
	}
#endif
	
	if(!sockok){
		DebugLog("%s:%d: connection failure", host, port);
		free(s);
		return NULL;
	}

	return (SOCKET_HANDLE)s;
}
