/* $Id$ */

%{
#include <fishymail.h>
#include <ui.tab.h>

static char* buf = NULL;
%}

%x DQ

%%

\" {
	buf = malloc(1);
	buf[0] = 0;
	BEGIN(DQ);
}

<DQ>[^"] {
	char* tmp = buf;
	buf = malloc(strlen(tmp) + strlen(yytext) + 1);
	strcpy(buf, tmp);
	strcat(buf, yytext);
	free(tmp);
}

<DQ>\\. {
	char* tmp = buf;
	buf = malloc(strlen(tmp) + 1 + 1);
	strcpy(buf, tmp);
	strcat(buf, yytext + 1);
	free(tmp);
}

<DQ>\" {
	strcpy(yylval.string, buf);
	BEGIN(INITIAL);
	free(buf);
	return STRING;
}

-?[0-9]+ {
	yylval.number = atoi(yytext);
	return NUMBER;
}

#[^\n]* {
	return COMMENT;
}

Set {
	return SET;
}

HelpPopup {
	return HELPPOPUP;
}

Popup	{
	return POPUP;
}

MenuItemSeparator {
	return MENUITEMSEPARATOR;
}

MenuItem {
	return MENUITEM;
}

Tree {
	return TREE;
}

[A-Za-z\./_\-]+ {
	return IDENTIFIER;
}

[\n]+ {
	return NEWLINE;
}

[ \t\v]+ {
	return SPACE;
}

. {
	return yytext[0];
}

%%
