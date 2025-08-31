#ifndef __FISHYMAIL_H__
#define __FISHYMAIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#define VERSION "1.00"

/* main.c */
void FishyMailMainRoutine(void);
void FishyMailMainUIRoutine(void);

/* ui driver */
void FishyMailShowMain(void);

/* Parser */
int yyparse(void);
int yylex(void);
int yyerror(const char* msg);

#endif
