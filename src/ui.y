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
%token POPUP SETUP HELPPOPUP MENUITEM SET
%start sequence

%union {
	char string[128];
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
		| SET SPACE IDENTIFIER '=' IDENTIFIER {
	/* ignore */
}

%%
