/* $Id$ */

#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <fishymail.h>
#include <pthread.h>

#define APP_CLASS "fishymail"

static pthread_t ui_thread;

static Widget	    top, button;
static XtAppContext app;

static char* fallback_resources[] = {
    APP_CLASS "*fontList: -sony-fixed-medium-r-normal--0-0-100-100-c-0-iso8859-1",
    NULL,
};

struct {
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

	FishyMailMainRoutine();

	ret = pthread_create(&ui_thread, NULL, ui_thread_routine, NULL);
	if(ret) {
		fprintf(stderr, "pthread error\n");
		return 1;
	}

	pthread_join(ui_thread, NULL);
	return 0;
}

void FishyMailShowMain(void) {
	top = XtVaAppInitialize(&app, APP_CLASS, NULL, 0,
				&args.argc, args.argv, fallback_resources, NULL);

	button = XmVaCreatePushButton(top, "hello", NULL);
	XtManageChild(button);

	XtRealizeWidget(top);
}
