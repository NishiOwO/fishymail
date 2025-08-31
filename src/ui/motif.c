/* $Id$ */

#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <fishymail.h>
#include <pthread.h>

static pthread_t ui_thread;

static Widget	    top, button;
static XtAppContext app;

struct {
	int    argc;
	char** argv;
} args;

static void* ui_thread_routine(void* arg) {
	FishyMailMainUIRoutine();
	XtAppMainLoop(app);
	return NULL;
}

int main(int argc, char** argv) {
	args.argc = argc;
	args.argv = argv;

	FishyMailMainRoutine();

	int ret = pthread_create(&ui_thread, NULL, ui_thread_routine, NULL);
	if(ret) {
		fprintf(stderr, "pthread error\n");
		return 1;
	}

	pthread_join(ui_thread, NULL);
	return 0;
}

void FishyMailShowMain(void) {
	top = XtVaAppInitialize(&app, args.argv[0], NULL, 0, &args.argc, args.argv, NULL, NULL);

	button = XmVaCreatePushButton(top, "hello", NULL);
	XtManageChild(button);

	XtRealizeWidget(top);
}
