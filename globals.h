#pragma once

#include <windows.h>
#include <windowsx.h>

#define APP_NAME	L"No-Wheel v0.2"

extern void				noweel_register_classes(HINSTANCE hInstance);
extern BOOL				InitInstance(HINSTANCE, int);
extern LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
extern void				start_hook();
extern void				stop_hook();
extern bool				is_hook_installed();
extern HWND mouseWnd;
extern HINSTANCE hInst;
extern LRESULT CALLBACK MouseWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
extern void create_mouse_wnd(int size, COLORREF color);
extern void destroy_mouse_wnd();

extern BOOL stopCursor;

#define NOWHEEL_CLASS		L"NOWHEEL"
#define NOWHEEL_MOUSE_CLASS	L"NOWHEEL_MOUSE"

#define WM_TRAYNOTIFY	(WM_USER + 100)

#define BNT_LRBUTTONS	0
#define BNT_RBUTTON		1
#define BNT_LBUTTON		2
#define BNT_MBUTTON		3
#define BNT_SHIFT		4
#define BNT_CTRL		5


#define MID_WEBSITE			1
#define MID_DONATE			2
#define MID_STARTUP			3
#define MID_STOPCURSOR		4
#define MID_SHOWMOUSE		5
#define MID_CIRCLE_SMALL	6
#define MID_CIRCLE_NORMAL	7
#define MID_CIRCLE_LARGE	8
#define MID_CIRCLE_COLOR	100
#define MID_EXIT			50

