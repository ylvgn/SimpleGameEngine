#if SGE_OS_WINDOWS

#include "PW5_MyMappingMode.h"

namespace sge {

void PW5_MyMappingMode::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);
}

void PW5_MyMappingMode::onDraw() {
	//_example1();
	//_example2();
	//_example3();
	//_example4();
	//_example5();
	_example6();
}

void PW5_MyMappingMode::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	// The mapping mode is said to define the mapping of the "window" (logical coordinates) to the "viewport" (device coordinates)
	// device units (which are pixels): device(viewport) coordinates
	// logical(window) coordinates: depends on mapping mode.

	// The window is specified in terms of logical coordinates,
	// which might be
		// pixels(MM_TEXT),
		// millimeters(MM_LOMETRIC/MM_HIMETRIC),
		// inches(MM_LOENGLISH/MM_HIENGLISH),
		// or any other unit you want(MM_TWIPS)

	// coordinates:
		// screen coordinates: GetWindowRect, GetCursorPos
			// The upper left corner of the screen is the point(0, 0)
		// Whole−window coordinates: GetWindowDC
			// refer to a program's entire application window,
			// including the title bar, menu, scroll bars, and border
			// upper left corner of the sizing border
		// client area coordinates: GetDC or BeginPaint
			// upper left corner of the client area

	// If you feel comfortable working in units of pixels,
	// you don't need to use any mapping modes except the default MM_TEXT mode.
	// If you need to display an image in inch or millimeter dimensions,
	// you can obtain the information you need from GetDeviceCaps and do your own scaling.
	// The other mapping modes are simply a convenient way to avoid doing your own scaling.
	hdc.setMappingMode(PW5_MappingMode::None); //::SetMapMode(hdc, MM_TEXT);
	hdc.textOut(0, 0, "MM_TEXT(units of pixels)");

	hdc.setMappingMode(PW5_MappingMode::LowEnglish); //::SetMapMode(hdc, MM_LOENGLISH);
	hdc.textOut(300, -50, "MM_LOENGLISH(.01 inch)");
	// x: 300 means 300 / 100 = 3 inch,
	// y: 50 / 100 = 0.5 inch, cuz y−coordinate down orientation is negative sign
	
	hdc.setMappingMode(PW5_MappingMode::HighEnglish); //::SetMapMode(hdc, MM_HIENGLISH);
	hdc.textOut(300*10, -50, "MM_HIENGLISH(.001 inch)");
	// x: 3000 means 3000 / 1000 = 3 inch,
	// y: 50 / 1000 = 0.05 inch

	hdc.setMappingMode(PW5_MappingMode::LowMetric); //::SetMapMode(hdc, MM_LOMETRIC);
	hdc.textOut(300, -50, "MM_LOMETRIC(.1 mm)");

	// 25.4 millimeters per inch, 25.4 mm. (25.396825) ~= 1 in.
	// so 320x240 mm. / 25.4 = 12.6x9.45 in.
	// when using MM_LOENGLISH to represent it, 12.6 in. will be described as 1260 (0.01 in.)

	hdc.setMappingMode(PW5_MappingMode::HighMetric); //::SetMapMode(hdc, MM_HIMETRIC);
	hdc.textOut(300*10, -50, "MM_HIMETRIC(.01 mm)");
	
	hdc.setMappingMode(PW5_MappingMode::Twips); //::SetMapMode(hdc, MM_TWIPS);
	// A "twip" is twentieth of a point, 1/20 point.
	// 1 point size = 1/72 inch
	// so 1/20 point = 1/72 / 20 = 1 / 1440 inch
	hdc.textOut(300, -50*10, "MM_TWIPS (1/1440 in)");
}

void PW5_MyMappingMode::_example2() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	// LogicalOrg == Window origin
	// DeviceOrg  == Viewport origin

	// DPtoLP
		// xDevice = xDeviceOrg + (xLogical-xLogicalOrg) * (xDeviceExt/xLogicalExt)
		// yDevice = yDeviceOrg + (yLogical-yLogicalOrg) * (yDeviceExt/yLogicalExt)

	// LPtoDP
		// xLogical = xLogicalOrg + (xDevice-xDeviceOrg) * (xLogicalExt/xDeviceExt)
		// yLogical = yLogicalOrg + (yDevice-yDeviceOrg) * (yLogicalExt/yDeviceExt)

	// when MM_TEXT, Window origin(0,0) and Viewport origin(0,0)
		// xViewport = xWindow − xWinOrg + xViewOrg
		// yViewport = yWindow − yWinOrg + yViewOrg

	Vec2f tmp{ 300, 100 }; // when tmp is fixed,
		// just change mapping mode, then use LPtoDP directly
		// is the same result with reset tmp(300,100) then use LPtoDP
	{
		hdc.setMappingMode(PW5_MappingMode::None);
		Vec2f p { 100, 100 };
		hdc.dPtoLP(p);
		hdc.Fmt_textOut(p, "Hello1");
		SGE_DUMP_VAR("Hello1", p);

		hdc.lPtoDP(tmp);
		auto logicalCoor = p;
		hdc.lPtoDP(p);
		hdc.Fmt_textOut(tmp, "Hello1: window(logical)={}, viewport(device)={}", logicalCoor, p);
	}

	// comment this scope to check whether is overlap
	{
		hdc.setMappingMode(PW5_MappingMode::LowEnglish);
		Vec2f p { 100, 100 };
		hdc.dPtoLP(p);
		hdc.Fmt_textOut(p, "Hello2");

		SGE_DUMP_VAR("Hello2", p);
		//Vec2f tmp{ 300, 100 };
		hdc.lPtoDP(tmp);
		auto logicalCoor = p;
		hdc.lPtoDP(p);
		hdc.Fmt_textOut(tmp, "Hello2: window(logical)={}, viewport(device)={}", logicalCoor, p);
	}
	// comment this scope to check whether is overlap
	{
		hdc.setMappingMode(PW5_MappingMode::HighEnglish);
		Vec2f p { 100, 100 };
		hdc.dPtoLP(p);
		hdc.Fmt_textOut(p, "Hello3");

		SGE_DUMP_VAR("Hello3", p);
		//Vec2f tmp{ 300, 100 };
		hdc.lPtoDP(tmp);
		auto logicalCoor = p;
		hdc.lPtoDP(p);
		hdc.Fmt_textOut(tmp, "Hello3: window(logical)={}, viewport(device)={}", logicalCoor, p);
	}
}

void PW5_MyMappingMode::_drawClientRectCoordinate(PW5_MappingMode v) {

	ScopedGetDC hdc(_hwnd);
	hdc.setMappingMode(v);

	if (v == PW5_MappingMode::None) {
		hdc.setViewportOrg(0, 0);
	} else {
		hdc.setViewportOrg(0, static_cast<int>(_clientRect.h));
	}

	::RECT rect;
	Rect2f rc;

	static Vec2f offset;

	::GetClientRect(_hwnd, &rect);
	GDI::dPtoLP(hdc, rect);

	using SRC = PW5_MappingMode;

	if (v == SRC::None) {
		Win32Util::convert(rc, rect);
	} else {
		rc.x = static_cast<float>(rect.left);
		rc.y = static_cast<float>(rect.bottom);
		rc.w = static_cast<float>(rect.right - rect.left);
		rc.h = static_cast<float>(rect.top - rect.bottom);
	}


	switch (v) {
		case SRC::LowMetric:	offset = { rc.w * 0.3f, 0 }; break;
		case SRC::HighMetric:	offset = { rc.w * 0.3f, rc.h * 0.3f }; break;
		case SRC::LowEnglish:	offset = { 0, 0 }; break;
		case SRC::HighEnglish:	offset = { 0, rc.h * 0.3f }; break;
		case SRC::Twips:		offset = { rc.w * 0.3f, rc.h * 0.6f }; break;
		case SRC::Isotropic:	offset = { rc.w * 0.6f, rc.h * 0.6f }; break;
		case SRC::Anisotropic:	offset = { rc.w * 0.6f, rc.h * 0.3f }; break;
	}

	if (v == PW5_MappingMode::None) {
		Vec2f from	{ rc.x,			rc.y + rc.h * 0.01f };
		Vec2f to	{ rc.w * 0.1f,	from.y };
		hdc.drawLine(from, to);
		hdc.Fmt_textOut(to, "X {}", rc.w);

		from = { rc.x + rc.w * 0.01f,	rc.y };
		to	 = { from.x,				rc.h * 0.1f };
		hdc.drawLine(from, to);
		hdc.Fmt_textOut(to, "Y {}", rc.h);
		hdc.Fmt_textOut(to + rc.w * 0.02f, enumStr(v));
	} else {
		Vec2f from	{ rc.x,	rc.w * 0.02f };		from += offset;
		Vec2f to	{ rc.w * 0.1f, from.y  };	to += offset;
		to.y = from.y;

		hdc.drawLine(from, to);
		hdc.Fmt_textOut(to, "X {}", rc.w);

		from = { rc.x + rc.w * 0.02f,	0 };			from += offset;
		to	 = { from.x,				rc.h * 0.1f };	to += offset;
		to.x = from.x;

		hdc.drawLine(from, to);
		hdc.Fmt_textOut(to, "Y {}", rc.h);
		hdc.Fmt_textOut(to + rc.w * 0.02f, enumStr(v));
	}
}

void PW5_MyMappingMode::_example3() {
	using SRC = PW5_MappingMode;

	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	_drawClientRectCoordinate(SRC::None);
	_drawClientRectCoordinate(SRC::LowMetric);
	_drawClientRectCoordinate(SRC::HighMetric);
	_drawClientRectCoordinate(SRC::LowEnglish);
	_drawClientRectCoordinate(SRC::HighEnglish);
	_drawClientRectCoordinate(SRC::Twips);
	_drawClientRectCoordinate(SRC::Isotropic);
	_drawClientRectCoordinate(SRC::Anisotropic);
}

void PW5_MyMappingMode::_example4() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	hdc.setViewportOrg({ _clientRect.w * 0.5f, _clientRect.h * 0.5f });

	Vec2f o { 0, 0 };
	hdc.textOut(o, "(0,0)");
	hdc.Fmt_textOut({o.x, o.y + 20.f}, "size = {}", _clientRect.size);

	{
		Vec2f from { o.x - _clientRect.w * 0.5f, 0 };
		Vec2f to { o.x + _clientRect.w * 0.5f, 0 };
		hdc.drawLine(from, to);
	}

	{
		Vec2f from{ 0, o.y - _clientRect.h * 0.5f };
		Vec2f to{ 0, o.y + _clientRect.h * 0.5f };
		hdc.drawLine(from, to);
	}

	{
		Vec2f devicePt = o;
		hdc.lPtoDP(devicePt);
		Vec2f logicalPt { 100.f, 100.f };
		// because textout is use logical coordinate, so we need to use o convert to logical coordinate
		Vec2f viewportOrg;
		hdc.getViewportOrg(viewportOrg);
		hdc.Fmt_textOut(devicePt - viewportOrg + logicalPt, "{} logical pt maps to {} device pt", logicalPt, devicePt);
		SGE_DUMP_VAR(devicePt);
	}

	{
		Vec2f pt { 0, 0 }; // when we want to define a device pt {0,0}, it should be left-top corner
		hdc.dPtoLP(pt); // and now it is in logical coordinate
		hdc.textOut(pt, "(0,0) device pt is left-top corner");
	}
}

void PW5_MyMappingMode::_example5() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	hdc.setViewportOrg({ _clientRect.w * 0.5f, _clientRect.h * 0.5f });
	hdc.setWindowOrg(  { -_clientRect.w * 0.5f, -_clientRect.h * 0.5f });

	// logical point (−_clientRect.w/2, −_clientRect.h/2) is mapped to the device point (_clientRect.x/2, _clientRect.h/2)
	/*
		now right-bottom corner is logical pt (0,0)
		-----------------------
						     -y
						      ^
						      ^
		------------ -x<<<<<(0,0)

		because at first line viewport pt is middle-center of logical coordinate
		and the secone line the logical middle-center org (0,0) changed to (-width*0.5, -height*0.5)
		it means now logical org needs to add (+width*0.5, +height*0.5) then will back to middle-center
		so it just like the following
			hdc.setViewportOrg({  _clientRect.w * 0.5f,  _clientRect.h * 0.5f });
			hdc.setWindowOrg  ({ -_clientRect.w * 0.5f, -_clientRect.h * 0.5f });
		is the same result with
			hdc.setViewportOrg({ _clientRect.w, _clientRect.h });
	*/
	Vec2f logicalPtOrg;
	hdc.getWindowOrg(logicalPtOrg);
	hdc.textOut(logicalPtOrg, "{0,0}");
	SGE_DUMP_VAR(logicalPtOrg);

	{
		// there is nothing change when you call multiply times this api
		hdc.setViewportOrg({ _clientRect.w * 0.5f, _clientRect.h * 0.5f });
		hdc.setViewportOrg({ _clientRect.w * 0.5f, _clientRect.h * 0.5f });
		hdc.setViewportOrg({ _clientRect.w * 0.5f, _clientRect.h * 0.5f });
		hdc.setViewportOrg({ _clientRect.w * 0.5f, _clientRect.h * 0.5f });

		// so if u want to reset the viewport org by using SetWindowOrg,
		// you need to do the as same opeartor as SetViewportOrg do !!
		Vec2f viewportOrg;
		hdc.getViewportOrg(viewportOrg);
		hdc.setWindowOrg(viewportOrg);

		hdc.textOut(0, 0, "(0,0) reset the org");

		SGE_DUMP_VAR("device:", viewportOrg);
		hdc.dPtoLP(viewportOrg);
		SGE_DUMP_VAR("logical:", viewportOrg);

		// now there is the side effect on logical coordinate, and GDI function is effect by logical coordinate
		// so there is nothing feel when we use TextOut, DrawText, etc..
		// and it just changed the Win32 saved internal data
	}

	{
		// now we reset the internal data by hard code with (0,0)
		// and do the same operaoter again with SetWindowOrg, it will reset back at the beginning (default org setting).
		hdc.setViewportOrg(0, 0);

		Vec2f viewportOrg;
		hdc.getViewportOrg(viewportOrg);
		hdc.setWindowOrg(viewportOrg);

		SGE_DUMP_VAR("device2:", viewportOrg);
		hdc.dPtoLP(viewportOrg);
		SGE_DUMP_VAR("logical2:", viewportOrg);
	}
}

void PW5_MyMappingMode::_example6() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	using SRC = PW5_MappingMode;

	// five mapping modes that express logical coordinates in physical measurements (physical units)
	_calcMappingModeExtRatio(SRC::LowMetric);	// lowest precision millimeters (0.1 mm.)
	_calcMappingModeExtRatio(SRC::HighMetric);  // highest precision millimeters (0.01 mm.)
	_calcMappingModeExtRatio(SRC::LowEnglish);  // lowest precision inches (0.01 in.)
	_calcMappingModeExtRatio(SRC::HighEnglish); // highest precision inches (0.001 in.)
	_calcMappingModeExtRatio(SRC::Twips);		// 1/20 point size (1/1440 in.)
}

void PW5_MyMappingMode::_calcMappingModeExtRatio(PW5_MappingMode v) {
	ScopedGetDC hdc(_hwnd);

	hdc.setMappingMode(v);

	auto ratioX = ::GetDeviceCaps(hdc, LOGPIXELSX);
	auto ratioY = ::GetDeviceCaps(hdc, LOGPIXELSY);
	SGE_DUMP_VAR("LOGPIXELSX", ratioX, "LOGPIXELSY", ratioY);

	// The viewport extents are based on the pixel dimensions of the screen.
	// This is information obtained from GetDeviceCaps using the HORZRES and VERTRES indexes
	auto viewportXExtents = ::GetDeviceCaps(hdc, HORZRES);
	auto viewportYExtents = ::GetDeviceCaps(hdc, VERTRES);
	SGE_DUMP_VAR("HORZRES", viewportXExtents, "VERTRES", viewportYExtents);

	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

	SGE_ASSERT(viewportXExtents == cxScreen);
	SGE_ASSERT(viewportYExtents == cyScreen);

	// The window extents are based on the assumed size of the display, which
	// GetDeviceCaps returns when you use the HORZSIZE and VERTSIZE indexes
	auto windowXExtents = ::GetDeviceCaps(hdc, HORZSIZE);
	auto windowYExtents = ::GetDeviceCaps(hdc, VERTSIZE);
	SGE_DUMP_VAR("HORZSIZE", windowXExtents, "VERTSIZE", windowYExtents);

	using SRC = PW5_MappingMode;

	Vec2f ext;
	hdc.getWindowExt(ext);
	SGE_DUMP_VAR(v, "getWindowExt", ext);
	switch (v)
	{
		case SRC::None:
			break;
		case SRC::LowMetric: // 0.1 mm.
			SGE_ASSERT(ext.x == windowXExtents * 10);
			SGE_ASSERT(ext.y == windowYExtents * 10);
			break;
		case SRC::HighMetric: // 0.01 mm.
			SGE_ASSERT(ext.x == windowXExtents * 100);
			SGE_ASSERT(ext.y == windowYExtents * 100);
			break;
		case SRC::LowEnglish:
			break;
		case SRC::HighEnglish:
			break;
		case SRC::Twips:
			break;
	}

	hdc.getViewportExt(ext);
	SGE_DUMP_VAR(v, "getViewportExt", ext);

	// let say pixel dimensions of display (display resolution) is 1024x768
	// in LOENGLISH, is mm. unit, let say is 320x240 mm.
		// viewportExt = (1024, -768) --> display resolution but y orientation is negative sign.
		// windowExt = (3200, 2400), cuz LOENGLISH is 0.1mm, so 3200 means actually 3200 * 0.1mm = 320 mm.
	// when it turns to HIENGLISH,
		// viewportExt = (1024, -768) still, cuz viewport is always use pixel unit
		// windowExt = (32000, 24000), cuz HIENGLISH is 0.01mm, so 32000 means actually 32000 * 0.01mm = 320 mm.

	// let say LOENGLISH, 0.01 in.
		// xViewExt/xWinExt = number of horizontal pixels in 0.01 in.
		// −yViewExt/yWinExt = negative number of vertical pixels in 0.01 in.

	// for MM_LOENGLISH, the ratio 96 divided by 100 is the number of pixels in 0.01 inches.For
	// MM_LOMETRIC, the ratio 96 divided by 254 is the number of pixels in 0.1 millimeters
		// 25.4 = 1 mm. so
	auto displayScaleRatio = GDI::getDisplayScaleRatio();
	if (v == SRC::None) {
		SGE_ASSERT(viewportXExtents * displayScaleRatio == ext.x);
		SGE_ASSERT(viewportYExtents * displayScaleRatio == ext.y);
	} else {
		SGE_ASSERT(viewportXExtents * displayScaleRatio == ext.x);
		SGE_ASSERT(viewportYExtents * displayScaleRatio == -ext.y);
	}
}

}

#endif