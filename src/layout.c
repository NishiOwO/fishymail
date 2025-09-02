/* $Id$ */
#include <fishymail.h>

#include <stb_ds.h>

typedef struct widget {
	void* opaque;
	int   left;
	int   top;
	int   right;
	int   bottom;
} widget_t;

static widget_t* widgets = NULL;

void FishyMailLayout(int ww, int wh) {
	int i;
	for(i = 0; i < arrlen(widgets); i++) {
		int x, y, w, h;
		x = widgets[i].left;
		y = widgets[i].top;
		w = widgets[i].right;
		h = widgets[i].bottom;

		if(x < 0) x = ww - x;
		if(y < 0) y = wh - y;
		if(w < 0) {
			w = -w;
		} else {
			w = ww - w;
		}
		if(h < 0) {
			h = -h;
		} else {
			h = wh - h;
		}

		w = w - x;
		h = h - y;

		FishyMailLayoutWidget(widgets[i].opaque, x, y, w, h);
	}
}

void FishyMailAddWidget(void* opaque, int left, int top, int right, int bottom) {
	widget_t w;

	w.opaque = opaque;
	w.left	 = left;
	w.top	 = top;
	w.right	 = right;
	w.bottom = bottom;

	arrput(widgets, w);
}
