/* $Id$ */

%{
#include <fishymail.h>

int yyerror(const char* error){
	(void)error;
	return 0;
}

int yywrap(void){
	return 1;
}
%}

%token STRING NEWLINE COMMENT SPACE IDENTIFIER NUMBER
%token POPUP SETUP HELPPOPUP MENUITEM SET MENUITEMSEPARATOR
%token TREE LIST READONLYTEXT TEXT
%start sequence

%union {
	char string[128];
	int number;
}

%%

sequence	: node
	 	| sequence node;

node		: command NEWLINE
      		| COMMENT NEWLINE;

command		: SPACE command_body
	 	| command_body;

command_body	: POPUP SPACE STRING {
	BeginPopup($<string>3, 0);
}
		| HELPPOPUP SPACE STRING {
	BeginPopup($<string>3, 1);
}
		| MENUITEM SPACE STRING {
	MenuItem($<string>3);
}
		| MENUITEMSEPARATOR {
	MenuItemSeparator();
}
		| TREE SPACE STRING SPACE NUMBER SPACE NUMBER SPACE NUMBER SPACE NUMBER {
	Tree($<string>3, $<number>5, $<number>7, $<number>9, $<number>11);
}
		| LIST SPACE STRING SPACE NUMBER SPACE NUMBER SPACE NUMBER SPACE NUMBER {
	List($<string>3, $<number>5, $<number>7, $<number>9, $<number>11);
}
		| READONLYTEXT SPACE STRING SPACE NUMBER SPACE NUMBER SPACE NUMBER SPACE NUMBER {
	ReadOnlyText($<string>3, $<number>5, $<number>7, $<number>9, $<number>11);
}
		| TEXT SPACE STRING SPACE NUMBER SPACE NUMBER SPACE NUMBER SPACE NUMBER {
	Text($<string>3, $<number>5, $<number>7, $<number>9, $<number>11);
}
		| SET SPACE IDENTIFIER '=' IDENTIFIER {
	/* ignore */
}

%%
