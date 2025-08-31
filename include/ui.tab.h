#ifndef _yy_defines_h_
#define _yy_defines_h_

#define STRING 257
#define NEWLINE 258
#define COMMENT 259
#define SPACE 260
#define IDENTIFIER 261
#define POPUP 262
#define SETUP 263
#define HELPPOPUP 264
#define MENUITEM 265
#define SET 266
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	char string[512];
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
