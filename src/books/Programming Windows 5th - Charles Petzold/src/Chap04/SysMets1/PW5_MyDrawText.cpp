#if SGE_OS_WINDOWS

#include "PW5_MyDrawText.h"


namespace sge {

static MyTextMetrics s_textMetrics;

template<class... Args>
inline void Fmt_drawText(::HDC hdc, int x, int y, const Args &... args) {
	auto s = Fmt(args...);
	const auto& tm = s_textMetrics;
	GDI::drawText(hdc, x, y, x + static_cast<int>(s.size() * tm.maxCharWidth), y + tm.aveCharHeight, s.view(), 0);
}

void PW5_MyDrawText::onCreate(CreateDesc& desc) {
	auto hInstance = ::GetModuleHandle(nullptr);
	const wchar_t* clsName = L"PW5_MyDrawText";

	auto wc = PW5_Win32Util::createWndClass(hInstance, clsName, s_wndProc);
	PW5_Win32Util::registerWndClass(wc);

	auto dwStyle = WS_OVERLAPPEDWINDOW;

	_hwnd = ::CreateWindowEx(0, clsName, clsName, dwStyle,
							 static_cast<int>(desc.rect.x),
							 static_cast<int>(desc.rect.y),
							 static_cast<int>(desc.rect.w),
							 static_cast<int>(desc.rect.h),
							 nullptr, nullptr, hInstance, nullptr);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

	::ShowWindow(_hwnd, SW_NORMAL);
	::UpdateWindow(_hwnd);
}

LRESULT CALLBACK PW5_MyDrawText::s_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_CREATE: {
			ScopedGetDC hdc(hwnd);
			s_textMetrics.create(hdc);
		}
		case WM_PAINT: {
			using DTFlag = ScopedPaintStruct::DTFlag;

			ScopedPaintStruct ps(hwnd);
			::RECT rc  = ps.rcPaint();
			int width  = rc.right - rc.left;
			int height = rc.bottom - rc.top;

			ScopedCreateSolidBrush scoped(ps, { 0, 51, 51, 255 } );
			ps.rectangle(rc.left, rc.top, rc.right, rc.bottom);

			ps.drawText(rc, "DT\r\nLEFT",				DTFlag::Left	| DTFlag::WordBreak);
			ps.drawText(rc, "DT\r\nCENTER",				DTFlag::Center	| DTFlag::WordBreak);
			ps.drawText(rc, "DT\r\nRIGHT",				DTFlag::Right	| DTFlag::WordBreak);
			ps.drawText(rc, "DT_LEFT | DT_VCENTER",		DTFlag::Left	| DTFlag::VCenter	| DTFlag::SingleLine);
			ps.drawText(rc, "DT_CENTER | DT_VCENTER",	DTFlag::Center	| DTFlag::VCenter	| DTFlag::SingleLine);
			ps.drawText(rc, "DT_RIGHT | DT_VCENTER",	DTFlag::Right	| DTFlag::VCenter	| DTFlag::SingleLine);
			ps.drawText(rc, "DT_LEFT | DT_BOTTOM",		DTFlag::Left	| DTFlag::Bottom	| DTFlag::SingleLine);
			ps.drawText(rc, "DT_CENTER | DT_BOTTOM ",	DTFlag::Center	| DTFlag::Bottom	| DTFlag::SingleLine);
			ps.drawText(rc, "DT_RIGHT | DT_BOTTOM",		DTFlag::Right	| DTFlag::Bottom	| DTFlag::SingleLine);

			{
				int x = rc.left + width / 4;
				int y = rc.top + height / 4;
				Fmt_drawText(ps.hdc(), x, y, "NorthWest");
			}
			{
				int x = rc.left + width * 3 / 4;
				int y = rc.top + height / 4;
				Fmt_drawText(ps.hdc(), x, y, "NorthEast");
			}
			{
				int x = rc.left + width / 4;
				int y = rc.top + height * 3/ 4;
				Fmt_drawText(ps.hdc(), x, y, "SouthWest");
			}
			{
				int x = rc.left + width * 3 / 4;
				int y = rc.top + height * 3 / 4;
				Fmt_drawText(ps.hdc(), x, y, "SouthEast");
			}
			{
				int x = rc.left + width / 2;
				int y = rc.top + height / 4;
				Fmt_drawText(ps.hdc(), x, y, "North");
			}
			{
				int x = rc.left + width * 3/ 4;
				int y = rc.top + height / 2;
				Fmt_drawText(ps.hdc(), x, y, "East");
			}
			{
				int x = rc.left + width / 4;
				int y = rc.top + height / 2;
				Fmt_drawText(ps.hdc(), x, y, "West");
			}
			{
				int x = rc.left + width / 2;
				int y = rc.top + height * 3 / 4;
				Fmt_drawText(ps.hdc(), x, y, "South");
			}
		} break;
	} // switch
    return DefWindowProc(hwnd, message, wParam, lParam);
}

}

#endif // SGE_OS_WINDOWS