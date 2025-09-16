/* $Id$ */
#include <fishymail.h>

#ifdef _WIN32
#include <winsock.h>
#include <windns.h>
#include <in6addr.h>
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
int FishyMailDNSInit(void) {
	return 0;
}

static WORD ToDNSAPIType(int type) {
	if(type == DNSPACKET_MX) return DNS_TYPE_MX;
	if(type == DNSPACKET_A) return DNS_TYPE_A;
	if(type == DNSPACKET_AAAA) return DNS_TYPE_AAAA;
	return -1;
}

void FishyMailDNSLookup(FishyMailDNSPacket_t* pkt, const char* host, int type) {
	PDNS_RECORD prec;
	pkt->count = 0;

	if(!DnsQuery_A(host, ToDNSAPIType(type), DNS_QUERY_STANDARD, NULL, &prec, NULL)) {
		/* https://ftp.zx.net.nz/pub/Patches/ftp.microsoft.com/MISC/KB/en-us/831/226.HTM */
		for(; prec != NULL; prec = prec->pNext) {
			if(type == DNSPACKET_MX) {
				/**
				 * XXX: appearantly broken on wine
				 *      but works on Windows 7
				 */
				if(pkt->count < MAXDNSPACKET) {
					pkt->result[pkt->count++] = malloc(strlen(prec->Data.MX.pNameExchange) + 1);
					strcpy(pkt->result[pkt->count - 1], prec->Data.MX.pNameExchange);
				}
			} else if(type == DNSPACKET_A) {
				if(pkt->count < MAXDNSPACKET) {
					struct in_addr* in	  = malloc(sizeof(struct in_addr));
					pkt->result[pkt->count++] = in;
					in->S_un.S_addr		  = prec->Data.A.IpAddress;
				}
			} else if(type == DNSPACKET_AAAA) {
				/* XXX: NOT SURE */
#if 0
				if(pkt->count < MAXDNSPACKET){
					struct in6_addr* in = malloc(sizeof(struct in6_addr));
					pkt->result[pkt->count++] = in;
					memcpy(&in->u, &prec->Data.AAAA.Ip6Address, sizeof(in->u));
				}
#endif
			}
		}
		DnsRecordListFree(prec, DnsFreeRecordListDeep);
	}
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
			if(pkt->count < MAXDNSPACKET) {
				char* mxname = malloc(MAXDNAME);
				dn_expand(ns_msg_base(handle), ns_msg_base(handle) + ns_msg_size(handle), ns_rr_rdata(rr) + NS_INT16SZ, mxname, sizeof(mxname));
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
