#include "globals.h"

HHOOK hMouseHook = 0;
HHOOK hKbdHook = 0;
int diff = 0;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && enableApp)
	{
		LPKBDLLHOOKSTRUCT hookData = (LPKBDLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:
			if (LCLickVK() == hookData->vkCode)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				mouse_event(MOUSEEVENTF_LEFTDOWN, ptCursor.x, ptCursor.y, 0, NULL);
				return TRUE;
			}
			else if (RCLickVK() == hookData->vkCode)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				mouse_event(MOUSEEVENTF_RIGHTDOWN, ptCursor.x, ptCursor.y, 0, NULL);
				return TRUE;
			}
			else if (MCLickVK() == hookData->vkCode)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				mouse_event(MOUSEEVENTF_MIDDLEDOWN, ptCursor.x, ptCursor.y, 0, NULL);
				return TRUE;
			}
			break;
		case WM_KEYUP:
			if (LCLickVK() == hookData->vkCode)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				mouse_event(MOUSEEVENTF_LEFTUP, ptCursor.x, ptCursor.y, 0, NULL);
				return TRUE;
			}
			else if (RCLickVK() == hookData->vkCode)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				mouse_event(MOUSEEVENTF_RIGHTUP, ptCursor.x, ptCursor.y, 0, NULL);
				return TRUE;
			}
			else if (MCLickVK() == hookData->vkCode)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				mouse_event(MOUSEEVENTF_MIDDLEUP, ptCursor.x, ptCursor.y, 0, NULL);
				return TRUE;
			}
			break;
		}
	}
	return CallNextHookEx(hKbdHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode >= 0)
	{
		LPMSLLHOOKSTRUCT lpMouse = (LPMSLLHOOKSTRUCT) lParam;
		switch (wParam)
		{
		case WM_MBUTTONDOWN:
			if (mapMiddleToRightButton)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				PostMessage(hWndMain, WM_MIDDLE_DOWN, ptCursor.x, ptCursor.y);
				return TRUE;
			}
		case WM_MBUTTONUP:
			if (mapMiddleToRightButton)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				PostMessage(hWndMain, WM_MIDDLE_UP, ptCursor.x, ptCursor.y);
				return TRUE;
			}
		case WM_MOUSEMOVE:
			if (enableApp)
			{
				POINT pt = lpMouse->pt;
				if ((GetKeyState(VK_RBUTTON) & 0x8000) && (GetKeyState(VK_LBUTTON) & 0x8000))
				{
					POINT ptCursor;
					GetCursorPos(&ptCursor);
					if (ptCursor.y != pt.y)
					{
						diff += ptCursor.y < pt.y ? pt.y - ptCursor.y : ptCursor.y - pt.y;
						if (diff > 10)
						{
							diff = 0;
							int delta = WHEEL_DELTA;
							if (ptCursor.y < pt.y)
							{
								delta = -WHEEL_DELTA;
							}
							mouse_event(MOUSEEVENTF_WHEEL, ptCursor.x, ptCursor.y, (DWORD)delta, NULL);
						}
					}
					if (stopCursor)
					{
						return TRUE;
					}
				}
				else
				{
					diff = 0;
				}
			}
			break;
		}
	}

	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void start_hook()
{
	diff = 0;
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
	hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
}

void stop_hook()
{
	if (hMouseHook)
	{
		UnhookWindowsHookEx(hMouseHook);
		hMouseHook = NULL;
	}
	if (hKbdHook)
	{
		UnhookWindowsHookEx(hKbdHook);
		hKbdHook = NULL;
	}
}

bool is_hook_installed()
{
	if (hMouseHook)
	{
		return true;
	}
	return false;
}
