#pragma once

#include <windows.h>
#include <windowsx.h>

#define APP_NAME	L"No-Wheel v0.5"

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
extern HWND hWndMain;

#define NOWHEEL_CLASS		L"NOWHEEL"
#define NOWHEEL_MOUSE_CLASS	L"NOWHEEL_MOUSE"

#define WM_TRAYNOTIFY	(WM_USER + 100)

#define WM_MIDDLE_DOWN	(WM_USER + 300)
#define WM_MIDDLE_UP	(WM_USER + 301)

#define MID_WEBSITE				1
#define MID_DONATE				2
#define MID_STARTUP				3
#define MID_STOPCURSOR			4
#define MID_SHOWMOUSE			5
#define MID_CIRCLE_SMALL		6
#define MID_CIRCLE_NORMAL		7
#define MID_CIRCLE_LARGE		8
#define MID_MIDDLE_TO_RIGHT		10
#define MID_CIRCLE_COLOR		100
#define MID_LCLICK				200
#define MID_RCLICK				300
#define MID_MCLICK				400
#define MID_EXIT				50

#define KBD_CLICK_NONE			0
#define KBD_CLICK_SCROLL		1
#define KBD_CLICK_RSHIFT		2
#define KBD_CLICK_RCTRL			3
#define KBD_CLICK_RALT			4
#define KBD_CLICK_PAUSE			5

extern int kbdLClick;
extern int kbdRClick;
extern int kbdMClick;
extern BOOL mapMiddleToRightButton;
extern BOOL enableApp;

inline int LCLickVK()
{
	switch (kbdLClick)
	{
	case KBD_CLICK_PAUSE:
		return VK_PAUSE;
	case KBD_CLICK_RALT:
		return VK_RMENU;
	case KBD_CLICK_SCROLL:
		return VK_SCROLL;
	case KBD_CLICK_RSHIFT:
		return VK_RSHIFT;
	case KBD_CLICK_RCTRL:
		return VK_RCONTROL;
	}
	return 0;
}

inline int RCLickVK()
{
	switch (kbdRClick)
	{
	case KBD_CLICK_PAUSE:
		return VK_PAUSE;
	case KBD_CLICK_RALT:
		return VK_RMENU;
	case KBD_CLICK_SCROLL:
		return VK_SCROLL;
	case KBD_CLICK_RSHIFT:
		return VK_RSHIFT;
	case KBD_CLICK_RCTRL:
		return VK_RCONTROL;
	}
	return 0;
}

inline int MCLickVK()
{
	switch (kbdMClick)
	{
	case KBD_CLICK_PAUSE:
		return VK_PAUSE;
	case KBD_CLICK_RALT:
		return VK_RMENU;
	case KBD_CLICK_SCROLL:
		return VK_SCROLL;
	case KBD_CLICK_RSHIFT:
		return VK_RSHIFT;
	case KBD_CLICK_RCTRL:
		return VK_RCONTROL;
	}
	return 0;
}
