#ifndef __FISHYMAIL_H__
#define __FISHYMAIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

typedef void* SOCKET_HANDLE;

#define MAXDNSPACKET 16

typedef struct FishyMailDNSPacket {
	int   count;
	void* result[MAXDNSPACKET];
} FishyMailDNSPacket_t;

enum FishyMailDNSPacket_type {
	DNSPACKET_MX = 0, /* char* */
	DNSPACKET_A,      /* struct in_addr */
	DNSPACKET_AAAA    /* struct in6_addr */
};

/* main.c */
int  FishyMailMainRoutine(void);
void FishyMailMainUIRoutine(void);
void FishyMailMenuItemPressed(const char* str);

/* util.c */
void FishyMailSanitizeName(const char* from, char* to);
void FishyMailRemoveSpecial(const char* from, char* to);

/* layout.c */
void FishyMailLayout(int ww, int wh);
void FishyMailAddWidget(void* opaque, int left, int top, int right, int bottom);

/* debug.c */
#ifdef DEBUG
void DebugInit(void);
void DebugLog(const char* fmt, ...);
#else
#define DebugInit()
#define DebugLog(...)
#endif

/* version.c */
extern const char* FishyMailVersion; /* when this returns only major.minor.patch */
				     /* ... */
char* FishyMailGetVersion(void);     /* this returns "full" version */

/* socket.c */
int FishyMailSocketInit(void);

/* dns.c */
int  FishyMailDNSInit(void);
void FishyMailDNSLookup(FishyMailDNSPacket_t* pkt, const char* host, int type);
void FishyMailFreeDNSPacket(FishyMailDNSPacket_t* pkt);

/* ui driver */
void FishyMailShowMain(void);
void FishyMailLayoutWidget(void* opaque, int x, int y, int w, int h);
void FishyMailStartQuit(void);

/* also ui driver, called from parser */
void BeginPopup(const char* name, int help);
void MenuItem(const char* name);
void MenuItemSeparator(void);
void Tree(const char* name, int left, int top, int right, int bottom);
void List(const char* name, int left, int top, int right, int bottom);
void ReadOnlyText(const char* name, int left, int top, int right, int bottom);
void Text(const char* name, int left, int top, int right, int bottom);

/* Parser */
void FishyMailPrepareUI(void);

int yyparse(void);
int yylex(void);
int yyerror(const char* msg);

#endif
