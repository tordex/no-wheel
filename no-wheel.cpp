#include "globals.h"
#include "resource.h"
#include <ShellAPI.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <strsafe.h>

/*
#pragma optimize("gsy",on)
#pragma comment(linker,"/MERGE:.rdata=.text")
#pragma comment(linker, "/SECTION:.text,EWR")
#pragma comment(linker,"/NODEFAULTLIB")
*/
//#pragma comment(linker,"/ENTRY:New_WinMain")

HINSTANCE hInst;
BOOL loadAtStartup;
BOOL stopCursor;
BOOL showMouse;
int  circleSize;
int  circleColor;
WCHAR settings_path[MAX_PATH];

struct
{
	COLORREF color;
	LPCWSTR name;
} circle_colors[] =
{
	{ RGB(255, 0, 0), L"Red Circle" },
	{ RGB(0, 255, 0), L"Green Circle" },
	{ RGB(0, 0, 255), L"Blue Circle" },
	{ RGB(255, 255, 0), L"Yellow Circle" },
};

BOOL CreatePath(LPCTSTR path)
{
	if (PathFileExists(path)) return TRUE;
	BOOL ret = FALSE;
	LPTSTR parentPath = new TCHAR[lstrlen(path) + 1];
	lstrcpy(parentPath, path);
	if (PathRemoveFileSpec(parentPath))
	{
		PathRemoveBackslash(parentPath);
		if (CreatePath(parentPath))
		{
			if (CreateDirectory(path, NULL))
			{
				ret = TRUE;
			}
		}
	}
	delete parentPath;
	return ret;
}

void load_settings()
{
	WCHAR path[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);
	PathAddBackslash(path);
	StringCchCat(path, MAX_PATH, L"Tordex\\No-Wheel");
	if (!PathFileExists(path))
	{
		CreatePath(path);
	}
	StringCchCat(path, MAX_PATH, L"\\settings.ini");
	stopCursor = GetPrivateProfileInt(L"settings", L"stopCursor", FALSE, path);
	circleSize = GetPrivateProfileInt(L"settings", L"circleSize", 0, path);
	circleColor = GetPrivateProfileInt(L"settings", L"circleColor", 0, path);
	showMouse = GetPrivateProfileInt(L"settings", L"showMouse", 0, path);
}

void save_settings()
{
	WCHAR path[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);
	PathAddBackslash(path);
	StringCchCat(path, MAX_PATH, L"Tordex\\No-Wheel\\settings.ini");

	WCHAR str[50];
	WritePrivateProfileString(L"settings", L"stopCursor", _itow(stopCursor, str, 10), path);
	WritePrivateProfileString(L"settings", L"circleSize", _itow(circleSize, str, 10), path);
	WritePrivateProfileString(L"settings", L"circleColor", _itow(circleColor, str, 10), path);
	WritePrivateProfileString(L"settings", L"showMouse", _itow(showMouse, str, 10), path);
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	load_settings();
	loadAtStartup = 0;
	mouseWnd = NULL;

	//hInst = (HINSTANCE) GetCurrentProcess();
	hInst = hInstance;

	HKEY hKeyMain = NULL;
	if(RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", NULL, KEY_READ, &hKeyMain) == ERROR_SUCCESS)
	{
		DWORD cbData = sizeof(DWORD);
		if(RegQueryValueEx(hKeyMain, L"No-Wheel", 0, NULL, NULL, &cbData) == ERROR_SUCCESS)
		{
			loadAtStartup = 1;
		} else
		{
			loadAtStartup = 0;
		}
		RegCloseKey(hKeyMain);
	}

	MSG msg;

	noweel_register_classes(hInst);

	if (!InitInstance(hInst, 0))
	{
		ExitProcess(1);
		return 0;
	}

	start_hook();

	if (showMouse)
	{
		create_mouse_wnd(circleSize, circle_colors[circleColor].color);
	}

	FreeEnvironmentStrings (GetEnvironmentStrings ());
	SetProcessWorkingSetSize (GetCurrentProcess (), -1, -1);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	stop_hook();

	ExitProcess(0);
	return 0;
}

void noweel_register_classes(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= NOWHEEL_CLASS;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)MouseWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = GetStockBrush(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = NOWHEEL_MOUSE_CLASS;
	wcex.hIconSm = NULL;

	RegisterClassEx(&wcex);

}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance;

   hWnd = CreateWindow(NOWHEEL_CLASS, L"No-Wheel", 0,
      0, 0, 0, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   return TRUE;
}

void update_tray_icon(HWND hWnd)
{
	NOTIFYICONDATA nd;
	RtlSecureZeroMemory(&nd, sizeof(nd));
	nd.cbSize = sizeof(nd);
	nd.hWnd = hWnd;
	nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nd.uCallbackMessage = WM_TRAYNOTIFY;
	if (is_hook_installed())
	{
		nd.hIcon = (HICON)LoadImage((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TRUEMOUSEWHEEL), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT | LR_DEFAULTCOLOR);
	}
	else
	{
		nd.hIcon = (HICON)LoadImage((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_DISABLED), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT | LR_DEFAULTCOLOR);
	}
	lstrcpy(nd.szTip, APP_NAME);
	Shell_NotifyIcon(NIM_MODIFY, &nd);
}

void register_tray_icon(HWND hWnd, BOOL reg)
{
	if(reg)
	{
		NOTIFYICONDATA nd;
		RtlSecureZeroMemory(&nd, sizeof(nd));
		nd.cbSize = sizeof(nd);
		nd.hWnd = hWnd;
		nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nd.uCallbackMessage = WM_TRAYNOTIFY;
		nd.hIcon = (HICON) LoadImage((HINSTANCE) GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TRUEMOUSEWHEEL), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT | LR_DEFAULTCOLOR);
		lstrcpy(nd.szTip, APP_NAME);
		Shell_NotifyIcon(NIM_ADD, &nd);
	} else
	{
		NOTIFYICONDATA nd;
		RtlSecureZeroMemory(&nd, sizeof(nd));
		nd.cbSize = sizeof(nd);
		nd.hWnd = hWnd;
		Shell_NotifyIcon(NIM_DELETE, &nd);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_TIMER:
		SetProcessWorkingSetSize (GetCurrentProcess (), -1, -1);
		break;
	case WM_CREATE:
		{
			register_tray_icon(hWnd, TRUE);
			SetTimer(hWnd, 1, 10000, NULL);
		}
		break;
	case WM_TRAYNOTIFY:
		{
			UINT msg = (UINT) lParam;
			switch(msg)
			{
			case WM_LBUTTONUP:
				if (is_hook_installed())
				{
					stop_hook();
				}
				else
				{
					start_hook();
				}
				update_tray_icon(hWnd);
				break;
			case WM_RBUTTONUP:
				{
					HMENU hMenu = CreatePopupMenu();
					InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, MID_WEBSITE, L"Web site..");
					InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, MID_DONATE, L"Donate...");
					InsertMenu(hMenu, -1, MF_SEPARATOR, 0, L"");
					InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING | (stopCursor ? MF_CHECKED : 0), MID_STOPCURSOR, L"Freeze Cursor");
					InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING | (showMouse ? MF_CHECKED : 0), MID_SHOWMOUSE, L"Show Mouse Circle");
					if (showMouse)
					{
						InsertMenu(hMenu, -1, MF_SEPARATOR, 0, L"");
						InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING | (circleSize == 0 ? MF_CHECKED : 0), MID_CIRCLE_SMALL, L"Small Circle");
						InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING | (circleSize == 1 ? MF_CHECKED : 0), MID_CIRCLE_NORMAL, L"Average Circle");
						InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING | (circleSize == 2 ? MF_CHECKED : 0), MID_CIRCLE_LARGE, L"Large Circle");
						InsertMenu(hMenu, -1, MF_SEPARATOR, 0, L"");
						int cmd = MID_CIRCLE_COLOR;
						for (auto clr : circle_colors)
						{
							InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING | ((circleColor == cmd - MID_CIRCLE_COLOR) ? MF_CHECKED : 0), cmd, clr.name);
							cmd++;
						}
						InsertMenu(hMenu, -1, MF_SEPARATOR, 0, L"");
					}
					InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING | (loadAtStartup ? MF_CHECKED : 0), MID_STARTUP, L"Run on startup");
					InsertMenu(hMenu, -1, MF_SEPARATOR, 0, L"");
					InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, MID_EXIT, L"Exit");
					POINT pt;
					GetCursorPos(&pt);
					SetForegroundWindow(hWnd);
					int ret = TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, NULL, hWnd, NULL);
					DestroyMenu(hMenu);
					switch(ret)
					{
					case MID_WEBSITE:
						ShellExecute(hWnd, NULL, L"http://www.tordex.com/", NULL, NULL, SW_SHOW);
						break;
					case MID_DONATE:
						ShellExecute(hWnd, NULL, L"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=FCPP4VYFHVZ2N", NULL, NULL, SW_SHOW);
						break;
					case MID_CIRCLE_SMALL:
						circleSize = 0;
						destroy_mouse_wnd();
						create_mouse_wnd(circleSize, circle_colors[circleColor].color);
						break;
					case MID_CIRCLE_NORMAL:
						circleSize = 1;
						destroy_mouse_wnd();
						create_mouse_wnd(circleSize, circle_colors[circleColor].color);
						save_settings();
						break;
					case MID_CIRCLE_LARGE:
						circleSize = 2;
						destroy_mouse_wnd();
						create_mouse_wnd(circleSize, circle_colors[circleColor].color);
						save_settings();
						break;
					case MID_SHOWMOUSE:
						if (showMouse)
						{
							destroy_mouse_wnd();
							showMouse = FALSE;
						}
						else
						{
							create_mouse_wnd(circleSize, circle_colors[circleColor].color);
							showMouse = TRUE;
						}
						save_settings();
						break;
					case MID_STOPCURSOR:
						stopCursor = stopCursor ? FALSE : TRUE;
						save_settings();
						break;
					case MID_STARTUP:
						{
							if(loadAtStartup)
							{
								loadAtStartup = 0;
							} else
							{
								loadAtStartup = 1;
							}
							HKEY hKeyMain = NULL;
							if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", NULL, KEY_WRITE, &hKeyMain) == ERROR_SUCCESS)
							{
								if(loadAtStartup)
								{
									WCHAR fileName[MAX_PATH];
									GetModuleFileName(GetModuleHandle(NULL), fileName, MAX_PATH);
									DWORD cbData = (lstrlen(fileName) + 1) * sizeof(WCHAR);
									RegSetValueEx(hKeyMain, L"No-Wheel", 0, REG_SZ, (LPBYTE)fileName, cbData);
								} else
								{
									RegDeleteValue(hKeyMain, L"No-Wheel");
								}
								RegCloseKey(hKeyMain);
							}
						}
						break;
					case MID_EXIT:
						DestroyWindow(hWnd);
						break;
					default:
						if (ret >= MID_CIRCLE_COLOR && ret < MID_CIRCLE_COLOR + sizeof(circle_colors) / sizeof(circle_colors[0]))
						{
							circleColor = ret - MID_CIRCLE_COLOR;
							destroy_mouse_wnd();
							create_mouse_wnd(circleSize, circle_colors[circleColor].color);
							save_settings();
						}
					}
				}
				break;
			}
		}
		break;
	case WM_DESTROY:
		{
			register_tray_icon(hWnd, FALSE);
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

