#include "globals.h"
#include "dib.h"

HWND mouseWnd;
int WND_SIZE = 48;

LRESULT CALLBACK MouseWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
		{
			POINT pt;
			GetCursorPos(&pt);
			SetWindowPos(mouseWnd, HWND_TOPMOST, pt.x - WND_SIZE / 2, pt.y - WND_SIZE / 2, WND_SIZE, WND_SIZE, SWP_NOACTIVATE | SWP_NOREDRAW);
		}
		break;
	case WM_CREATE:
		{
			SetTimer(hWnd, 1, 10, NULL);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

inline void PreMulRGBA(RGBQUAD& color)
{
	color.rgbRed = (color.rgbRed		* color.rgbReserved) / 255;
	color.rgbGreen = (color.rgbGreen	* color.rgbReserved) / 255;
	color.rgbBlue = (color.rgbBlue	* color.rgbReserved) / 255;
}


void create_mouse_wnd(int size, COLORREF color)
{
	if (mouseWnd) return;

	switch (size)
	{
	case 0:
		WND_SIZE = 16;
		break;
	case 1:
		WND_SIZE = 32;
		break;
	default:
		WND_SIZE = 48;
		break;
	}

	POINT pt;
	GetCursorPos(&pt);
	mouseWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		NOWHEEL_MOUSE_CLASS, L"No-Wheel-Mouse", WS_POPUP,
		pt.x - WND_SIZE / 2, pt.y - WND_SIZE / 2, WND_SIZE, WND_SIZE, NULL, NULL, hInst, NULL);

	WCHAR res_name[100];
	wsprintf(res_name, L"circle-%d", WND_SIZE);

	HBITMAP bmp = (HBITMAP)LoadBitmap(hInst, res_name);
	if (bmp)
	{
		HDC screendc = GetDC(0);
		HDC hdc = CreateCompatibleDC(screendc);
		HBITMAP oldbmp = (HBITMAP)SelectObject(hdc, bmp);
		ReleaseDC(0, screendc);

		simpledib::dib dib;
		dib.create(WND_SIZE, WND_SIZE, true);
		
		BitBlt(dib, 0, 0, WND_SIZE, WND_SIZE, hdc, 0, 0, SRCCOPY);

		SelectObject(hdc, oldbmp);
		DeleteObject(bmp);
		DeleteDC(hdc);

		for (int i = 0; i < WND_SIZE * WND_SIZE; i++)
		{
			dib.bits()[i].rgbBlue = GetBValue(color);
			dib.bits()[i].rgbGreen = GetGValue(color);;
			dib.bits()[i].rgbRed = GetRValue(color);;
			PreMulRGBA(dib.bits()[i]);
		}

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.SourceConstantAlpha = 100;

		POINT	pt_dst = { 0, 0 };
		SIZE	sz_dst = { WND_SIZE, WND_SIZE };
		POINT	pt_src = { 0, 0 };

		UpdateLayeredWindow(mouseWnd, 0, 0, &sz_dst, dib, &pt_src, 0, &bf, ULW_ALPHA);
	}

	ShowWindow(mouseWnd, SW_SHOW);
}

void destroy_mouse_wnd()
{
	if (!mouseWnd) return;
	DestroyWindow(mouseWnd);
	mouseWnd = NULL;
}