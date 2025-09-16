/* $Id$ */
#include <fishymail.h>

#ifdef _WIN32
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>
#endif

void FishyMailFreeDNSPacket(FishyMailDNSPacket_t* pkt) {
	int i;
	for(i = 0; i < pkt->count; i++) free(pkt->result[i]);
}

#ifdef _WIN32
void FishyMailDNSInit(void) {
}

void FishyMailDNSLookup(FishyMailDNSPacket_t* pkt, const char* host, int type) {
}
#else
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

static int ToBINDType(int type) {
	if(type == DNSPACKET_MX) return T_MX;
	if(type == DNSPACKET_A) return T_A;
	if(type == DNSPACKET_AAAA) return T_AAAA;
	return -1;
}

void FishyMailDNSLookup(FishyMailDNSPacket_t* pkt, const char* host, int type) {
	ns_msg	      handle;
	unsigned char answer[NS_MAXMSG];
	int	      len = res_nquery(&statp, host, C_IN, ToBINDType(type), answer, sizeof(answer));
	int	      i;
	ns_rr	      rr;

	pkt->count = 0;

	ns_initparse(answer, len, &handle);
	for(i = 0; i < ns_msg_count(handle, ns_s_an); i++) {
		if(ns_parserr(&handle, ns_s_an, i, &rr)) break;
		if(ns_rr_type(rr) == ns_t_mx && type == DNSPACKET_MX) {
			char* mxname = malloc(MAXDNAME);
			dn_expand(ns_msg_base(handle), ns_msg_base(handle) + ns_msg_size(handle), ns_rr_rdata(rr) + NS_INT16SZ, mxname, sizeof(mxname));

			if(pkt->count < MAXDNSPACKET) {
				pkt->result[pkt->count++] = mxname;
			}
		} else if(ns_rr_type(rr) == ns_t_a && type == DNSPACKET_A) {
			if(pkt->count < MAXDNSPACKET) {
				pkt->result[pkt->count++] = malloc(sizeof(struct in_addr));
				memcpy(pkt->result[pkt->count - 1], ns_rr_rdata(rr), sizeof(struct in_addr));
			}
		} else if(ns_rr_type(rr) == ns_t_aaaa && type == DNSPACKET_AAAA) {
			if(pkt->count < MAXDNSPACKET) {
				pkt->result[pkt->count++] = malloc(sizeof(struct in6_addr));
				memcpy(pkt->result[pkt->count - 1], ns_rr_rdata(rr), sizeof(struct in6_addr));
			}
		}
	}
}
#endif
