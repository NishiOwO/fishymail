#ifndef _yy_defines_h_
#define _yy_defines_h_

#define STRING 257
#define NEWLINE 258
#define COMMENT 259
#define SPACE 260
#define IDENTIFIER 261
#define NUMBER 262
#define POPUP 263
#define SETUP 264
#define HELPPOPUP 265
#define MENUITEM 266
#define SET 267
#define MENUITEMSEPARATOR 268
#define TREE 269
#define LIST 270
#define READONLYTEXT 271
#define TEXT 272
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE {
	char string[128];
	int number;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
