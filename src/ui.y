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

%token STRING NEWLINE COMMENT SPACE IDENTIFIER
%token POPUP SETUP HELPPOPUP MENUITEM
%start sequence

%union {
	char string[512];
}

%%

sequence	: node
	 	| sequence node;

node		: command NEWLINE
      		| COMMENT NEWLINE;

command		: SPACE command_body
	 	| command_body;

command_body	: POPUP SPACE STRING {
}
		| HELPPOPUP SPACE STRING {
}
		| MENUITEM SPACE STRING {
}
		| SET SPACE IDENTIFIER '=' IDENTIFIER {
}

%%
