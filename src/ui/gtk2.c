/* $Id$ */

#include <fishymail.h>

#include <gtk/gtk.h>

static pthread_t  ui_thread;
static GtkWidget* top;
static GtkWidget* menubar;
static GtkWidget* vbox;
static GtkWidget* fixed;

void FishyMailStartQuit(void) {
	gtk_main_quit();
}

static gboolean resize_callback(GtkWindow* win, GdkEvent* event, gpointer data) {
	GtkRequisition req;

	(void)win;
	(void)data;

	gtk_widget_size_request(menubar, &req);
	gtk_widget_set_size_request(fixed, event->configure.width, event->configure.height - req.height);

	FishyMailLayout(event->configure.width, event->configure.height - req.height);

	return FALSE;
}

static void menuitem_pressed(GtkMenuItem* menuitem, gpointer data) {
	(void)menuitem;

	FishyMailMenuItemPressed(data);
}

static void* ui_thread_routine(void* arg) {
	(void)arg;

	FishyMailMainUIRoutine();

	g_signal_connect(G_OBJECT(top), "configure-event", G_CALLBACK(resize_callback), NULL);

	gtk_main();

	return NULL;
}

int main(int argc, char** argv) {
	int ret;
	int st;

	DebugInit();

	if((st = FishyMailMainRoutine()) != 0) return st;

	gtk_init(&argc, &argv);

	top = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(top), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(top), 800, 600);
	gtk_window_set_title(GTK_WINDOW(top), "FishyMail");
	gtk_widget_add_events(top, GDK_CONFIGURE);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(top), vbox);

	menubar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

	fixed = gtk_fixed_new();
	gtk_box_pack_start(GTK_BOX(vbox), fixed, TRUE, TRUE, 0);

	ret = pthread_create(&ui_thread, NULL, ui_thread_routine, NULL);
	if(ret != 0) {
		fprintf(stderr, "pthread error\n");
		return 1;
	}

	pthread_join(ui_thread, NULL);
	return 0;
}

void FishyMailShowMain(void) {
	gtk_widget_show_all(top);
}

void FishyMailLayoutWidget(void* opaque, int x, int y, int w, int h) {
	GtkWidget* widget = (GtkWidget*)opaque;

	gtk_fixed_move(GTK_FIXED(fixed), widget, x, y);
	gtk_widget_set_size_request(widget, w, h);
}

void FishyMailShowVersion(void) {
	GtkWidget* dialog;
	char	   buf[512];

	sprintf(buf, "FishyMail Version %s", FishyMailVersion);

	dialog = gtk_message_dialog_new(GTK_WINDOW(top),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_OK,
					buf);
	gtk_window_set_title(GTK_WINDOW(dialog), "Version Information");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static GtkWidget* menu;

static void ToGTKMnemonic(const char* in, char* out) {
	int i;
	int incr = 0;
	for(i = 0; in[i] != 0; i++) {
		if(in[i] == '&') {
			out[incr++] = '_';
			out[incr++] = in[++i];
		} else {
			out[incr++] = in[i];
		}
	}
	out[incr] = 0;
}

char menubuf[128];

void BeginPopup(const char* name, int help) {
	GtkWidget* menuname;
	char	   idname[256];

	sprintf(menubuf, "%s", name);

	ToGTKMnemonic(name, idname);

	menu = gtk_menu_new();

	menuname = gtk_menu_item_new_with_mnemonic(idname);

	if(help) {
		gtk_menu_item_set_right_justified(GTK_MENU_ITEM(menuname), TRUE);
	}

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuname), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuname);
}

void MenuItem(const char* name) {
	GtkWidget* menuname;
	char	   idname[256];
	char	   tmp[256];
	char*	   buffer = malloc(256);

	sprintf(tmp, "MENU_%s_%s", menubuf, name);
	FishyMailSanitizeName(tmp, buffer);

	ToGTKMnemonic(name, idname);

	menuname = gtk_menu_item_new_with_mnemonic(idname);
	g_signal_connect(G_OBJECT(menuname), "activate", G_CALLBACK(menuitem_pressed), buffer);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuname);
}

void MenuItemSeparator(void) {
	GtkWidget* sep;

	sep = gtk_separator_menu_item_new();

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep);
}

void Tree(const char* name, int left, int top, int right, int bottom) {
	GtkWidget* w	 = gtk_tree_view_new();
	GtkWidget* frame = gtk_scrolled_window_new(NULL, NULL);

	(void)name;

	gtk_container_set_border_width(GTK_CONTAINER(frame), 2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frame), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(frame), w);

	gtk_fixed_put(GTK_FIXED(fixed), frame, left, top);

	FishyMailAddWidget(frame, left, top, right, bottom);
}

void List(const char* name, int left, int top, int right, int bottom) {
	GtkWidget* w	 = gtk_tree_view_new();
	GtkWidget* frame = gtk_scrolled_window_new(NULL, NULL);

	(void)name;

	gtk_container_set_border_width(GTK_CONTAINER(frame), 2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frame), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(frame), w);

	gtk_fixed_put(GTK_FIXED(fixed), frame, left, top);

	FishyMailAddWidget(frame, left, top, right, bottom);
}

void ReadOnlyText(const char* name, int left, int top, int right, int bottom) {
	GtkWidget* w	 = gtk_text_view_new();
	GtkWidget* frame = gtk_scrolled_window_new(NULL, NULL);

	(void)name;

	gtk_container_set_border_width(GTK_CONTAINER(frame), 2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frame), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(frame), w);

	gtk_text_view_set_editable(GTK_TEXT_VIEW(w), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), frame, left, top);

	FishyMailAddWidget(frame, left, top, right, bottom);
}

void Text(const char* name, int left, int top, int right, int bottom) {
	GtkWidget* w	 = gtk_text_view_new();
	GtkWidget* frame = gtk_scrolled_window_new(NULL, NULL);

	(void)name;

	gtk_container_set_border_width(GTK_CONTAINER(frame), 2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frame), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(frame), w);

	gtk_fixed_put(GTK_FIXED(fixed), frame, left, top);

	FishyMailAddWidget(frame, left, top, right, bottom);
}
