/* $Id$ */

#include <fishymail.h>

#include <stb_ds.h>

#include <windows.h>
#include <commctrl.h>

static HINSTANCE hInst;
static HANDLE	 hUIThread;
static HANDLE	 hUIReady;
static HMENU	 hMenu;
static HWND	 hMain;

typedef struct id {
	char* key;
	int   value;
} id_t;

static id_t* ids = NULL;
void	     FishyMailStartQuit(void) {
	DestroyWindow(hMain);
}

static int GetIDFromName(const char* name) {
	int ind = shgeti(ids, name);
	if(ind == -1) return -1;

	return ids[ind].value;
}

static int AllocateIDFromName(const char* name) {
	int v = 100 + shlen(ids);
	shput(ids, name, v);

	return v;
}

static void ShowBitmapSize(HDC hdc, const char* name, int x, int y, int w, int h) {
	HBITMAP hBitmap = LoadBitmap(hInst, name);
	BITMAP	bmp;
	HDC	hmdc;
	GetObject(hBitmap, sizeof(bmp), &bmp);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hBitmap);
	if(w == 0 && h == 0) {
		StretchBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	} else {
		StretchBlt(hdc, x, y, w, h, hmdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}
	DeleteDC(hmdc);
	DeleteObject(hBitmap);
}

static DWORD WINAPI UIThread(LPVOID lp) {
	BOOL	  bret;
	MSG	  msg;
	RECT	  rc;
	const int ww = 800;
	const int wh = 600;

	(void)lp;

	hMain = CreateWindow("FishyMailMain", "FishyMail", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, ww, wh, NULL, 0, hInst, NULL);
	if(hMain == NULL) {
		SetEvent(hUIReady);
		return 0;
	}

	ShowWindow(hMain, SW_NORMAL);
	UpdateWindow(hMain);

	FishyMailMainUIRoutine();

	GetClientRect(hMain, &rc);
	FishyMailLayout(rc.right - rc.left, rc.bottom - rc.top);

	SetMenu(hMain, hMenu);
	DrawMenuBar(hMain);

	SetEvent(hUIReady);

	while((bret = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if(bret == -1) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	if(msg == WM_CLOSE) {
		DestroyWindow(hWnd);
	} else if(msg == WM_COMMAND) {
		int m = LOWORD(wp);
		int i;
		for(i = 0; i < shlen(ids); i++) {
			if(ids[i].value == m && strlen(ids[i].key) > 5 && memcmp(ids[i].key, "MENU_", 5) == 0) {
				FishyMailMenuItemPressed(ids[i].key);
				break;
			}
		}
	} else if(msg == WM_CTLCOLORSTATIC) {
		return DefWindowProc(hWnd, WM_CTLCOLOREDIT, wp, lp);
	} else if(msg == WM_DESTROY) {
		PostQuitMessage(0);
	} else if(msg == WM_SIZE) {
		RECT rc;
		GetClientRect(hWnd, &rc);
		FishyMailLayout(rc.right - rc.left, rc.bottom - rc.top);
	} else {
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}

typedef struct splash {
	HDC	hDC;
	HBITMAP hBitmap;
} splash_t;
static LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	if(msg == WM_DESTROY) {
		splash_t* s = (splash_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		DeleteDC(s->hDC);
		DeleteObject(s->hBitmap);
		free(s);
	} else if(msg == WM_CREATE) {
		RECT	  rc;
		HDC	  dc;
		DWORD	  d = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
		splash_t* s = malloc(sizeof(*s));

		GetClientRect(hWnd, &rc);

		dc = GetDC(hWnd);

		s->hBitmap = CreateCompatibleBitmap(dc, rc.right - rc.left, rc.bottom - rc.top);
		s->hDC	   = CreateCompatibleDC(dc);
		SelectObject(s->hDC, s->hBitmap);

		ReleaseDC(hWnd, dc);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)s);

		if(!(d & WS_CAPTION)) {
			SetTimer(hWnd, 100, 3 * 1000, NULL);
		}
	} else if(msg == WM_TIMER || msg == WM_LBUTTONDOWN) {
		DestroyWindow(hWnd);
	} else if(msg == WM_ERASEBKGND) {
	} else if(msg == WM_PAINT) {
		PAINTSTRUCT ps;
		RECT	    rc;
		HDC	    hDC;
		char	    buf[128];
		splash_t*   s = (splash_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		GetClientRect(hWnd, &rc);

		ShowBitmapSize(s->hDC, "SPLASH", 0, 0, rc.right - rc.left, rc.bottom - rc.top);

		sprintf(buf, "Version %s", VERSION);
		SetBkMode(s->hDC, TRANSPARENT);
		SetTextAlign(s->hDC, TA_BOTTOM | TA_RIGHT);
		SetTextColor(s->hDC, RGB(0xff, 0xff, 0xff));
		TextOut(s->hDC, rc.right - rc.left - 8, rc.bottom - rc.top - 8, buf, strlen(buf));

		hDC = BeginPaint(hWnd, &ps);
		StretchBlt(hDC, 0, 0, rc.right - rc.left, rc.bottom - rc.top, s->hDC, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SRCCOPY);
		EndPaint(hWnd, &ps);
	} else {
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}

static BOOL InitClass(const char* name, WNDPROC proc) {
	WNDCLASSEX wc;
	wc.cbSize	 = sizeof(wc);
	wc.style	 = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc	 = proc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInst;
	wc.hIcon	 = LoadIcon(hInst, "FISHYMAIL");
	wc.hCursor	 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_MENU);
	wc.lpszMenuName	 = NULL;
	wc.lpszClassName = name;
	wc.hIconSm	 = LoadIcon(hInst, "FISHYMAIL");

	return RegisterClassEx(&wc);
}

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow) {
	DWORD id;

	(void)hPrevInst;
	(void)lpsCmdLine;
	(void)nCmdShow;

	DebugInit();

	hInst = hCurInst;

	if(!InitClass("FishyMailMain", MainWndProc)) return 0;
	if(!InitClass("FishyMailSplash", SplashWndProc)) return 0;

	hMenu = CreateMenu();
	sh_new_strdup(ids);

	InitCommonControls();

	if(FishyMailMainRoutine() != 0) return 0;

	hUIReady = CreateEvent(NULL, FALSE, FALSE, NULL);

	hUIThread = CreateThread(NULL, 0, UIThread, NULL, 0, &id);

	WaitForSingleObject(hUIReady, INFINITE);
	CloseHandle(hUIReady);

	WaitForSingleObject(hUIThread, INFINITE);

	return 0;
}

void FishyMailShowMain(void) {
}

static HMENU hPopupMenu;
static char  PopupName[128];
static int   PopupPosition = 0;

void BeginPopup(const char* name, int help) {
	sprintf(PopupName, "MENU_%s", name);

	hPopupMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_POPUP | MF_STRING, (UINT_PTR)hPopupMenu, name);

	if(help) {
		MENUITEMINFO mii;

		mii.cbSize     = sizeof(mii);
		mii.fMask      = MIIM_TYPE;
		mii.dwTypeData = (char*)name;
		mii.fType      = MFT_RIGHTJUSTIFY | MFT_STRING;

		SetMenuItemInfo(hMenu, PopupPosition, TRUE, &mii);
	}

	PopupPosition++;
}

void MenuItem(const char* name) {
	char idname[128];
	char tmp[128];

	sprintf(tmp, "%s_%s", PopupName, name);
	FishyMailSanitizeName(tmp, idname);

	AppendMenu(hPopupMenu, MF_STRING, AllocateIDFromName(idname), name);
}

void MenuItemSeparator(void) {
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, NULL);
}

void FishyMailLayoutWidget(void* opaque, int x, int y, int w, int h) {
	MoveWindow((HWND)opaque, x, y, w, h, TRUE);
}

void Tree(const char* name, int left, int top, int right, int bottom) {
	char idname[128];
	char tmp[128];
	HWND wnd;

	sprintf(tmp, "TREE_%s", name);
	FishyMailSanitizeName(tmp, idname);

	wnd = CreateWindow(WC_TREEVIEW, "", WS_CHILD | WS_BORDER | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT, 0, 0, 0, 0, hMain, (HMENU)(ULONG_PTR)AllocateIDFromName(idname), hInst, NULL);

	FishyMailAddWidget(wnd, left, top, right, bottom);
}

void List(const char* name, int left, int top, int right, int bottom) {
	char idname[128];
	char tmp[128];
	HWND wnd;

	sprintf(tmp, "LIST_%s", name);
	FishyMailSanitizeName(tmp, idname);

	wnd = CreateWindow("LISTBOX", "", WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hMain, (HMENU)(ULONG_PTR)AllocateIDFromName(idname), hInst, NULL);

	FishyMailAddWidget(wnd, left, top, right, bottom);
}

void ReadOnlyText(const char* name, int left, int top, int right, int bottom) {
	char idname[128];
	char tmp[128];
	HWND wnd;

	sprintf(tmp, "READONLYTEXT_%s", name);
	FishyMailSanitizeName(tmp, idname);

	wnd = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_READONLY, 0, 0, 0, 0, hMain, (HMENU)(ULONG_PTR)AllocateIDFromName(idname), hInst, NULL);

	FishyMailAddWidget(wnd, left, top, right, bottom);
}

void Text(const char* name, int left, int top, int right, int bottom) {
	char idname[128];
	char tmp[128];
	HWND wnd;

	sprintf(tmp, "TEXT_%s", name);
	FishyMailSanitizeName(tmp, idname);

	wnd = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE, 0, 0, 0, 0, hMain, (HMENU)(ULONG_PTR)AllocateIDFromName(idname), hInst, NULL);

	FishyMailAddWidget(wnd, left, top, right, bottom);
}
