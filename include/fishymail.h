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
void FishyMailRemoveSpecial(const char* from, char* to);

/* layout.c */
void FishyMailLayout(int ww, int wh);
void FishyMailAddWidget(void* opaque, int left, int top, int right, int bottom);

/* ui driver */
void FishyMailShowMain(void);
void FishyMailLayoutWidget(void* opaque, int x, int y, int w, int h);

/* also ui driver, called from parser */
void BeginPopup(const char* name, int help);
void MenuItem(const char* name);
void MenuItemSeparator(void);
void Tree(const char* name, int left, int top, int right, int bottom);

/* Parser */
void FishyMailPrepareUI(void);

int yyparse(void);
int yylex(void);
int yyerror(const char* msg);

#endif
