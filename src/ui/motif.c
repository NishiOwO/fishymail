/* $Id$ */

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>

#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/MainW.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>

#include <fishymail.h>
#include <pthread.h>

#define APP_CLASS "fishymail"

static pthread_t ui_thread;

static Widget	    top, mainw, button, menubar;
static XtAppContext app;

static char* fallback_resources[] = {
    APP_CLASS "*fontList: -sony-fixed-medium-r-normal--0-0-100-100-c-0-iso8859-1",
    NULL,
};

static struct {
	int    argc;
	char** argv;
} args;

static void* ui_thread_routine(void* arg) {
	(void)arg;

	FishyMailMainUIRoutine();
	XtAppMainLoop(app);
	return NULL;
}

int main(int argc, char** argv) {
	int ret;

	args.argc = argc;
	args.argv = argv;

	top = XtVaAppInitialize(&app, APP_CLASS, NULL, 0,
				&args.argc, args.argv, fallback_resources, XtNtitle, "FishyMail", NULL);

	XtResizeWidget(top, 800, 600, 1);

	mainw = XtVaCreateManagedWidget("MainWindow", xmMainWindowWidgetClass, top, NULL);

	menubar = XmVaCreateSimpleMenuBar(mainw, "MenuBar", NULL);
	XtManageChild(menubar);

	XtRealizeWidget(top);

	FishyMailMainRoutine();

	ret = pthread_create(&ui_thread, NULL, ui_thread_routine, NULL);
	if(ret != 0) {
		fprintf(stderr, "pthread error\n");
		return 1;
	}

	pthread_join(ui_thread, NULL);
	return 0;
}

void FishyMailShowMain(void) {
	button = XmVaCreatePushButton(mainw, "hello", NULL);
	XtManageChild(button);
}

static Widget popup_menu;
static char   popup_name[128];

void BeginPopup(const char* name, int help) {
	char	 tmp[128];
	char	 idname[128];
	char	 menuname[128];
	Widget	 menu;
	XmString str;
	char	 c = 0;
	int	 i;

	sprintf(popup_name, "%s", name);

	for(i = 0; name[i] != 0; i++) {
		if(name[i] == '&') {
			c = name[i + 1];
		}
	}

	sprintf(tmp, "MENU_%s", name);
	FishyMailSanitizeName(tmp, idname);

	sprintf(tmp, "%s", name);
	FishyMailRemoveSpecial(tmp, menuname);

	popup_menu = XmCreatePulldownMenu(menubar, idname, NULL, 0);

	str  = XmStringCreateLocalized(menuname);
	menu = XmVaCreateCascadeButton(menubar, idname,
				       XmNsubMenuId, popup_menu,
				       XmNlabelString, str,
				       XmNmnemonic, c,
				       NULL);
	XmStringFree(str);

	if(help) {
		Arg arg[1];
		XtSetArg(arg[0], XmNmenuHelpWidget, menu);
		XtSetValues(menubar, arg, 1);
	}

	XtManageChild(menu);
}

void MenuItem(const char* name) {
	char	 tmp[256];
	char	 idname[128];
	char	 menuname[128];
	Widget	 w;
	XmString str;
	char	 c = 0;
	int	 i;

	for(i = 0; name[i] != 0; i++) {
		if(name[i] == '&') {
			c = name[i + 1];
		}
	}

	sprintf(tmp, "MENU_%s_%s", popup_name, name);
	FishyMailSanitizeName(tmp, idname);

	sprintf(tmp, "%s", name);
	FishyMailRemoveSpecial(tmp, menuname);

	str = XmStringCreateLocalized(menuname);
	w   = XmVaCreatePushButton(popup_menu, idname,
				   XmNlabelString, str,
				   XmNmnemonic, c,
				   NULL);
	XmStringFree(str);
	XtManageChild(w);

	for(i = 0; name[i] != 0; i++) {
		if(name[i] == '&') {
			c = name[i + 1];
		}
	}
}
