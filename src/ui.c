/**
 * This file is auto-generated using ./tools/gui.pl
 * DO NOT EDIT MANUALLY
 */

#include <fishymail.h>
#include <ui.tab.h>
const char* ui_text = \
	"# $Id$\n" \
	"# FishyMail UI stuff, read tools/gui.pl to understand\n" \
	"# vim: ft=text\n" \
	"#\n" \
	"# +-----+-------------------+\n" \
	"# |     |     Mail list     |\n" \
	"# |  T  |                   |\n" \
	"# |  r  +-------------------+\n" \
	"# |  e  |    Mail content   |\n" \
	"# |  e  |                   |\n" \
	"# |     |                   |\n" \
	"# +-----+-------------------+\n" \
	"#\n" \
	"\n" \
	"Set Title=FishyMail\n" \
	"Set EmbedFile=src/ui.c\n" \
	"\n" \
	"Popup \"&File\"\n" \
	"	MenuItem \"&Quit\"\n" \
	"\n" \
	"Popup \"&Edit\"\n" \
	"\n" \
	"Popup \"&View\"\n" \
	"\n" \
	"HelpPopup \"&Help\"\n" \
	"	MenuItem \"&Version\"\n" \
	"\n" \
	"Tree \"TREE\" 0 0 -200 0\n" \
	"List \"MAIL\" 200 0 0 -100\n" \
	"ReadOnlyText \"TEXT\" 200 100 0 0\n";

extern void* yy_scan_string(const char* str);
extern void yy_delete_buffer(void* buffer);
void FishyMailPrepareUI(void){
	void* buf = yy_scan_string(ui_text);
	if(yyparse() != 0){
		yy_delete_buffer(buf);
		return;
	}
	yy_delete_buffer(buf);
}
