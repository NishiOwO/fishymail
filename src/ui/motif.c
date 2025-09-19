/* $Id$ */

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/xpm.h>
#include <X11/X.h>

#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/MainW.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/Outline.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>

#include "../images/fishymail.xpm"
#include <fishymail.h>
#include <pthread.h>

#define APP_CLASS "fishymail"
#define TEXT_KEYBIND "#override Ctrl <Key> C: copy-clipboard()\\nCtrl <Key> V: paste-clipboard()\\nCtrl <Key> A: select-all()\\nCtrl <Key> X: cut-clipboard()"

static pthread_t ui_thread;

static Widget	    top, mainw, form, menubar;
static XtAppContext app;
static Pixmap	    icon_pixmap, icon_mask;

static char* fallback_resources[] = {
    APP_CLASS "*fontList: -sony-fixed-medium-r-normal--16-120-100-100-c-80-iso8859-1",
    APP_CLASS "*MenuBar*fontList: *-helvetica-bold-o-normal-*-14-*-iso8859-1",
    APP_CLASS "*TEXT_TEXT*translations: " TEXT_KEYBIND,
    NULL,
};

static struct {
	int    argc;
	char** argv;
} args;

static Atom quit_atom;

void FishyMailStartQuit(void) {
	XEvent ev;

	ev.xclient.type		= ClientMessage;
	ev.xclient.send_event	= True;
	ev.xclient.message_type = quit_atom;
	ev.xclient.format	= 32;
	ev.xclient.serial	= 0;
	ev.xclient.window	= XtWindow(top);
	ev.xclient.data.l[0]	= quit_atom;

	XSendEvent(XtDisplay(top), XtWindow(top), False, NoEventMask, &ev);
}

static void* ui_thread_routine(void* arg) {
	XEvent ev;

	(void)arg;

	FishyMailMainUIRoutine();

	XtWidgetGeometry geo, geo2;
	XtQueryGeometry(mainw, NULL, &geo);
	XtQueryGeometry(menubar, NULL, &geo2);

	FishyMailLayout(geo.width, geo.height - geo2.height);

	while(1) {
		XtAppNextEvent(app, &ev);
		XtDispatchEvent(&ev);

		if(ev.type == ResizeRequest || ev.type == ConfigureNotify) {
			XtQueryGeometry(mainw, NULL, &geo);
			XtQueryGeometry(menubar, NULL, &geo2);

			FishyMailLayout(geo.width, geo.height - geo2.height);
		} else if(ev.type == ClientMessage && ev.xclient.message_type == quit_atom) {
			DebugLog("Quit atom received");
			break;
		}
	}

	return NULL;
}

int main(int argc, char** argv) {
	int	      ret;
	int	      st;
	char	      bg[64];
	unsigned long color;

	args.argc = argc;
	args.argv = argv;

	XInitThreads();
	DebugInit();

	top = XtVaAppInitialize(&app, APP_CLASS, NULL, 0,
				&args.argc, args.argv, fallback_resources, XtNtitle, "FishyMail", NULL);

	quit_atom = XInternAtom(XtDisplay(top), "_FISHYMAIL_QUIT", False);

	mainw = XtVaCreateManagedWidget("MainWindow", xmMainWindowWidgetClass, top, NULL);

	XtVaGetValues(mainw, XtNbackground, &color, NULL);
	sprintf(bg, "  c #%.6lx", color);

	fishymail[1] = bg;

	XpmCreatePixmapFromData(XtDisplay(top), DefaultRootWindow(XtDisplay(top)), fishymail, &icon_pixmap, &icon_mask, NULL);
	XtVaSetValues(top, XmNiconPixmap, icon_pixmap, NULL);
	XtVaSetValues(top, XmNiconMask, icon_mask, NULL);

	menubar = XmCreateMenuBar(mainw, "MenuBar", NULL, 0);
	XtManageChild(menubar);

	form = XtVaCreateWidget("Form", xmFormWidgetClass, mainw, NULL);
	XtManageChild(form);

	XtResizeWidget(top, 800, 600, 1);

	if((st = FishyMailMainRoutine()) != 0) return st;

	ret = pthread_create(&ui_thread, NULL, ui_thread_routine, NULL);
	if(ret != 0) {
		fprintf(stderr, "pthread error\n");
		return 1;
	}

	pthread_join(ui_thread, NULL);
	return 0;
}

void FishyMailShowMain(void) {
	XtRealizeWidget(top);
}

static Widget popup_menu;
static char   popup_name[128];

static void MenuItemCallback(Widget w, XtPointer ptr, XtPointer data) {
	(void)ptr;
	(void)data;

	FishyMailMenuItemPressed(XtName(w));
}

void FishyMailLayoutWidget(void* opaque, int x, int y, int w, int h) {
	XtVaSetValues((Widget)opaque, XmNleftAttachment, XmATTACH_NONE, XmNtopAttachment, XmATTACH_NONE, XmNx, x, XmNy, y, NULL);
	XtVaSetValues((Widget)opaque, XmNwidth, w, XmNheight, h, NULL);
}

void FishyMailShowVersion(void) {
	XmString title = XmStringCreateLocalized("Version Information");
	Widget	 dialog, d_form, d_logo;
	Arg	 args[2];

	XtSetArg(args[0], XmNdialogTitle, title);
	XtSetArg(args[1], XmNnoResize, True);

	dialog = XmCreateMessageDialog(top, "DialogInfo", args, 2);

	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

	d_form = XtVaCreateWidget("DialogInfoForm", xmFormWidgetClass, dialog, NULL);
	d_logo = XtVaCreateWidget("DialogInfoLogo", xmLabelWidgetClass, d_form,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNlabelType, XmPIXMAP,
				  XmNlabelPixmap, icon_pixmap,
				  NULL);
	XtManageChild(d_logo);
	XtManageChild(d_form);

	XmStringFree(title);
	XtManageChild(dialog);
}

void BeginPopup(const char* name, int help) {
	char	 tmp[128];
	char	 idname[128];
	char	 menuname[128];
	Widget	 menu;
	XmString str;
	Arg	 arg[1];
	char	 c = 0;
	int	 i;

	sprintf(popup_name, "%s", name);

	for(i = 0; name[i] != 0; i++) {
		if(name[i] == '&') {
			c = name[i + 1];
			break;
		}
	}

	sprintf(tmp, "MENU_%s", name);
	FishyMailSanitizeName(tmp, idname);

	sprintf(tmp, "%s", name);
	FishyMailRemoveSpecial(tmp, menuname);

	popup_menu = XmCreatePulldownMenu(menubar, idname, NULL, 0);

	str  = XmStringCreateLocalized(menuname);
	menu = XtVaCreateWidget(idname, xmCascadeButtonWidgetClass, menubar,
				XmNlabelString, str,
				XmNsubMenuId, popup_menu,
				NULL);

	if(help) {
		XtSetArg(arg[0], XmNmenuHelpWidget, menu);
		XtSetValues(menubar, arg, 1);
	}

	XtSetArg(arg[0], XmNmnemonic, c);
	XtSetValues(menu, arg, 1);

	XtManageChild(menu);
	XmStringFree(str);
}

void MenuItem(const char* name) {
	char	 tmp[256];
	char	 idname[128];
	char	 menuname[128];
	Widget	 w;
	XmString str;
	Arg	 arg[1];
	char	 c = 0;
	int	 i;

	for(i = 0; name[i] != 0; i++) {
		if(name[i] == '&') {
			c = name[i + 1];
			break;
		}
	}

	sprintf(tmp, "MENU_%s_%s", popup_name, name);
	FishyMailSanitizeName(tmp, idname);

	sprintf(tmp, "%s", name);
	FishyMailRemoveSpecial(tmp, menuname);

	str = XmStringCreateLocalized(menuname);
	w   = XtVaCreateWidget(idname, xmPushButtonWidgetClass, popup_menu,
			       XmNlabelString, str,
			       NULL);
	XtAddCallback(w, XmNactivateCallback, MenuItemCallback, NULL);

	XtSetArg(arg[0], XmNmnemonic, c);
	XtSetValues(w, arg, 1);

	XtManageChild(w);
	XmStringFree(str);
}

void MenuItemSeparator(void) {
	Widget w = XtVaCreateWidget("SEPARATOR", xmSeparatorWidgetClass, popup_menu, NULL);
	XtManageChild(w);
}

void Tree(const char* name, int left, int top, int right, int bottom) {
	char   tmp[256];
	char   idname[128];
	Widget w, f;

	sprintf(tmp, "TREEFRAME_%s", name);
	FishyMailSanitizeName(tmp, idname);

	f = XtVaCreateWidget(idname, xmFrameWidgetClass, form, NULL);
	XtManageChild(f);

	sprintf(tmp, "TREE_%s", name);
	FishyMailSanitizeName(tmp, idname);

	w = XtVaCreateWidget(idname, xmOutlineWidgetClass, f, NULL);
	XtManageChild(w);

	FishyMailAddWidget(f, left, top, right, bottom);
}

void List(const char* name, int left, int top, int right, int bottom) {
	char   tmp[256];
	char   idname[128];
	Widget w, f;

	sprintf(tmp, "LISTFRAME_%s", name);
	FishyMailSanitizeName(tmp, idname);

	f = XtVaCreateWidget(idname, xmScrolledWindowWidgetClass, form, XmNscrollBarPlacement, XmBOTTOM_RIGHT, NULL);
	XtManageChild(f);

	sprintf(tmp, "LIST_%s", name);
	FishyMailSanitizeName(tmp, idname);

	w = XtVaCreateWidget(idname, xmListWidgetClass, f, XmNscrollBarDisplayPolicy, XmSTATIC, NULL);
	XtManageChild(w);

	FishyMailAddWidget(f, left, top, right, bottom);
}

void ReadOnlyText(const char* name, int left, int top, int right, int bottom) {
	char   tmp[256];
	char   idname[128];
	Widget w, f;

	sprintf(tmp, "READONLYTEXTFRAME_%s", name);
	FishyMailSanitizeName(tmp, idname);

	f = XtVaCreateWidget(idname, xmScrolledWindowWidgetClass, form, NULL);
	XtManageChild(f);

	sprintf(tmp, "READONLYTEXT_%s", name);
	FishyMailSanitizeName(tmp, idname);

	w = XtVaCreateWidget(idname, xmTextWidgetClass, f, XmNeditMode, XmMULTI_LINE_EDIT, XmNeditable, FALSE, NULL);
	XtManageChild(w);

	FishyMailAddWidget(f, left, top, right, bottom);
}

void Text(const char* name, int left, int top, int right, int bottom) {
	char   tmp[256];
	char   idname[128];
	Widget w, f;

	sprintf(tmp, "TEXTFRAME_%s", name);
	FishyMailSanitizeName(tmp, idname);

	f = XtVaCreateWidget(idname, xmScrolledWindowWidgetClass, form, NULL);
	XtManageChild(f);

	sprintf(tmp, "TEXT_%s", name);
	FishyMailSanitizeName(tmp, idname);

	w = XtVaCreateWidget(idname, xmTextWidgetClass, f, XmNeditMode, XmMULTI_LINE_EDIT, NULL);
	XtManageChild(w);

	FishyMailAddWidget(f, left, top, right, bottom);
}
