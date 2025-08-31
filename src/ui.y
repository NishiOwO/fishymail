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
%%

a: a

%%
