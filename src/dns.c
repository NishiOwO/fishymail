/* $Id$ */
#include <fishymail.h>

#ifdef _WIN32
void FishyMailDNSInit(void) {
}
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#define MAXDNSPKT 16

typedef struct dnspkt {
	int   count;
	void* result[MAXDNSPKT];
} dnspkt_t;

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

static void FreeDNSPKT(dnspkt_t* pkt) {
	int i;
	for(i = 0; i < pkt->count; i++) free(pkt->result[i]);
}

static void DNSLookup(dnspkt_t* pkt, const char* host, int type) {
	ns_msg	      handle;
	unsigned char answer[NS_MAXMSG];
	int	      len = res_nquery(&statp, host, C_IN, type, answer, sizeof(answer));
	int	      i;
	ns_rr	      rr;

	pkt->count = 0;

	ns_initparse(answer, len, &handle);
	for(i = 0; i < ns_msg_count(handle, ns_s_an); i++) {
		if(ns_parserr(&handle, ns_s_an, i, &rr)) break;
		if(ns_rr_type(rr) == ns_t_mx && type == T_MX) {
			char* mxname = malloc(MAXDNAME);
			dn_expand(ns_msg_base(handle), ns_msg_base(handle) + ns_msg_size(handle), ns_rr_rdata(rr) + NS_INT16SZ, mxname, sizeof(mxname));

			if(pkt->count < MAXDNSPKT) {
				pkt->result[pkt->count++] = mxname;
			}
		} else if(ns_rr_type(rr) == ns_t_a && type == T_A) {
			if(pkt->count < MAXDNSPKT) {
				pkt->result[pkt->count++] = malloc(sizeof(struct in_addr));
				memcpy(pkt->result[pkt->count - 1], ns_rr_rdata(rr), sizeof(struct in_addr));
			}
		} else if(ns_rr_type(rr) == ns_t_aaaa && type == T_AAAA) {
			if(pkt->count < MAXDNSPKT) {
				pkt->result[pkt->count++] = malloc(sizeof(struct in6_addr));
				memcpy(pkt->result[pkt->count - 1], ns_rr_rdata(rr), sizeof(struct in6_addr));
			}
		}
	}
}
#endif
