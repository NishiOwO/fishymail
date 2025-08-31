#ifndef __FISHYMAIL_H__
#define __FISHYMAIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#define VERSION "1.00"

/* main.c */
void FishyMailMainRoutine(void);
void FishyMailMainUIRoutine(void);

/* util.c */
void FishyMailSanitizeName(const char* from, char* to);

/* ui driver */
void FishyMailShowMain(void);

/* also ui driver, called from parser */
void BeginPopup(const char* name, int help);
void MenuItem(const char* name);

/* Parser */
void FishyMailPrepareUI(void);

int yyparse(void);
int yylex(void);
int yyerror(const char* msg);

#endif
