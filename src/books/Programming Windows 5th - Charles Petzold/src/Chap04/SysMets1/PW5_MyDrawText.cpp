#include "PW5_MyDrawText.h"

#if SGE_OS_WINDOWS

namespace sge {

LRESULT CALLBACK PW5_MyDrawText::s_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_PAINT: {
		ScopedPaintStruct ps(hwnd);
		RECT rc = ps.rcPaint();
			
		auto brush = CreateSolidBrush(RGB(0, 200, 200));
		SelectObject(ps, brush);
		ps.rectangle(rc);

		g_drawText(ps, &rc, L"DT\r\nLEFT", DT_LEFT | DT_WORDBREAK);
		g_drawText(ps, &rc, L"DT\r\nCENTER", DT_CENTER | DT_WORDBREAK);
		g_drawText(ps, &rc, L"DT\r\nRIGHT", DT_RIGHT | DT_WORDBREAK);

		g_drawText(ps, &rc, L"DT_LEFT | DT_VCENTER", DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		g_drawText(ps, &rc, L"DT_CENTER | DT_VCENTER", DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		g_drawText(ps, &rc, L"DT_RIGHT | DT_VCENTER", DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

		g_drawText(ps, &rc, L"DT_LEFT | DT_BOTTOM", DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		g_drawText(ps, &rc, L"DT_CENTER | DT_BOTTOM ", DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		g_drawText(ps, &rc, L"DT_RIGHT | DT_BOTTOM", DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
			
		DeleteObject(brush);
	} break;
    case WM_DESTROY:
        PostQuitMessage (0);
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS