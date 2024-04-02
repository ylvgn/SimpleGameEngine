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
	// A "twip" is 1/20 point and hence 1/1440 inch
		// I mentioned earlier that a point is a unit of measurement in typography that is approximately 1 / 72 inch
		// but that is often assumed in graphics programming to be exactly 1/72 inch
	// so in program, 1/20 point size = 1/72 / 20 = 1 / 1440 inch
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
		// xViewport = xWindow - xWinOrg + xViewOrg
		// yViewport = yWindow - yWinOrg + yViewOrg

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
		Vec2f to   { o.x + _clientRect.w * 0.5f, 0 };
		hdc.drawLine(from, to);
	}

	{
		Vec2f from { 0, o.y - _clientRect.h * 0.5f };
		Vec2f to   { 0, o.y + _clientRect.h * 0.5f };
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

	hdc.setViewportOrg({ _clientRect.w * 0.5f,  _clientRect.h * 0.5f });
	hdc.setWindowOrg({  -_clientRect.w * 0.5f, -_clientRect.h * 0.5f });

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
		// there is nothing change when you call setViewportOrg multiply times
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

		SGE_DUMP_VAR("device1:", viewportOrg);
		hdc.dPtoLP(viewportOrg);
		SGE_DUMP_VAR("logical1:", viewportOrg);

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

	{
		// and now we want Cartesian coordinate system
		// Solution1 is using setViewportOrg
		hdc.setViewportOrg({_clientRect.w * 0.5f, _clientRect.h * 0.5f});
		/*
			-----------------------
						+y
						^
				  -----------+x

			-----------------------
		*/
		hdc.textOut(0, 0, "Cartesian coordinate");

		Vec2f viewportOrg;
		Vec2f windowtOrg;

		hdc.getViewportOrg(viewportOrg);
		SGE_DUMP_VAR("device3-1:", viewportOrg);

		hdc.getWindowOrg(windowtOrg);
		SGE_DUMP_VAR("local3-1:", windowtOrg);

		// And Solution2 is using setWindowOrg
		// before that, we reset viewportOrg to left-top(0,0)
		hdc.setViewportOrg(0, 0);
		hdc.setWindowOrg(0, 0);

		// now change origin with setWindowOrg

		// You can also use the SetWindowOrgEx function to change the logical(0, 0) point,
		// but the task is a little more difficult because the arguments to SetWindowOrgEx have to be in logical coordinates.
		// You would first need to convert(cxClient, cyClient) to a logical coordinate using the DPtoLP function.
		// Assuming that the variable pt is a structure of type POINT,
		// this code changes the logical(0, 0) point to the center of the client area
		Vec2f p = _clientRect.size * 0.5f;
		hdc.dPtoLP(p);
		hdc.setWindowOrg(p);

		hdc.getViewportOrg(viewportOrg);
		SGE_DUMP_VAR("device3-2:", viewportOrg);

		hdc.getWindowOrg(windowtOrg);
		SGE_DUMP_VAR("local3-2:", windowtOrg);
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
	_calcMappingModeExtRatio(SRC::Text);		// pixel unit
}

void PW5_MyMappingMode::_calcMappingModeExtRatio(PW5_MappingMode v) {
	using SRC = PW5_MappingMode;

	ScopedGetDC hdc(_hwnd);
	hdc.setMappingMode(v);

	// Windows uses the HORZRES,HORZSIZE,VERTRES,and VERTSIZE values
	// when calculating windowand offset extents for the various mapping modes

	// The viewport extents are based on the pixel dimensions of the screen.
	// This is information obtained from GetDeviceCaps using the HORZRES and VERTRES indexes
	Vec2i viewportExtInPixel {
		::GetDeviceCaps(hdc, HORZRES),
		::GetDeviceCaps(hdc, VERTRES)
	};
	SGE_DUMP_VAR(v, viewportExtInPixel);

	Vec2i screenResolution {
		::GetSystemMetrics(SM_CXSCREEN),
		::GetSystemMetrics(SM_CYSCREEN)
	};
	SGE_ASSERT(viewportExtInPixel == screenResolution);

	// The window extents are based on the assumed size of the display, which
	// GetDeviceCaps returns when you use the HORZSIZE and VERTSIZE indexes
	Vec2i windowExtInMillimeters {
		::GetDeviceCaps(hdc, HORZSIZE),
		::GetDeviceCaps(hdc, VERTSIZE)
	};
	SGE_DUMP_VAR(v, windowExtInMillimeters);

	Vec2i viewportExtInUnit;
	hdc.getViewportExt(viewportExtInUnit);
	SGE_DUMP_VAR(v, viewportExtInUnit);

	switch (v)
	{
		case SRC::Text: { // pixel unit
			Vec2i windowExtInText;
			hdc.getWindowExt(windowExtInText);
			SGE_ASSERT(windowExtInText.x == viewportExtInUnit.x == 1);
			SGE_ASSERT(windowExtInText.y == viewportExtInUnit.y == 1);
		} break;
		case SRC::LowMetric: { // 0.1 mm.
			Vec2i windowExtInLowMetric;
			hdc.getWindowExt(windowExtInLowMetric);
			SGE_ASSERT(windowExtInLowMetric == windowExtInMillimeters * 10);
		} break;
		case SRC::HighMetric: { // 0.01 mm.
			Vec2i windowExtInHighMetric;
			hdc.getWindowExt(windowExtInHighMetric);
			SGE_ASSERT(windowExtInHighMetric == windowExtInMillimeters * 100);
		} break;
		case SRC::LowEnglish: { // 0.01 in.
			Vec2i windowExtInLowEnglish;
			hdc.getWindowExt(windowExtInLowEnglish);
			Vec2f windowExtInInch = GDI::mmToInch(windowExtInMillimeters);
			SGE_ASSERT(windowExtInLowEnglish.x == Math::round(windowExtInInch.x * 100));
			SGE_ASSERT(windowExtInLowEnglish.y == Math::round(windowExtInInch.y * 100));
		} break;
		case SRC::HighEnglish: { // 0.001 in.
			Vec2i windowExtInHighEnglish;
			hdc.getWindowExt(windowExtInHighEnglish);
			auto windowExtInInch = GDI::mmToInch(windowExtInMillimeters);
			SGE_ASSERT(windowExtInHighEnglish.x == Math::round(windowExtInInch.x * 1000));
			SGE_ASSERT(windowExtInHighEnglish.y == Math::round(windowExtInInch.y * 1000));
		} break;
		case SRC::Twips: { // 1/20 point size (1/1440 in.)
			Vec2i windowExtInTwips;
			hdc.getWindowExt(windowExtInTwips);
			SGE_DUMP_VAR(v, windowExtInTwips);
			auto windowExtInTwips2 = GDI::mmToTwips(windowExtInMillimeters);
			SGE_ASSERT(windowExtInTwips.x == Math::round(windowExtInTwips2.x));
			SGE_ASSERT(windowExtInTwips.y == Math::round(windowExtInTwips2.y));
		} break;
	}

	if (v != SRC::Text) {
		float displayScaleRatio = GDI::getDisplayScaleRatio();
		int x = static_cast<int>(viewportExtInPixel.x * displayScaleRatio);
		int y = static_cast<int>(viewportExtInPixel.y * displayScaleRatio);
		SGE_ASSERT(x == viewportExtInUnit.x);
		SGE_ASSERT(y == -viewportExtInUnit.y);
	}
}

}

#endif