#include "globals.h"

HHOOK hHook = 0;
int diff = 0;


LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode >= 0)
	{
		LPMSLLHOOKSTRUCT lpMouse = (LPMSLLHOOKSTRUCT) lParam;
		if(wParam == WM_MOUSEMOVE)
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
						mouse_event(MOUSEEVENTF_WHEEL, ptCursor.x, ptCursor.y, (DWORD) delta, NULL);
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
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void start_hook()
{
	diff = 0;
	hHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
}

void stop_hook()
{
	if(hHook)
	{
		UnhookWindowsHookEx(hHook);
		hHook = NULL;
	}
}

bool is_hook_installed()
{
	if (hHook)
	{
		return true;
	}
	return false;
}
