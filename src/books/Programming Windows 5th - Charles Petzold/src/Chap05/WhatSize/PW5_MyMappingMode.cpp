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
	_example3();
}

void PW5_MyMappingMode::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	// device units (which are pixels): device coordinates
		// screen coordinates:
	// logical units: depends on mapping mode.

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

	::POINT p = { 100, 100 };
	hdc.Fmt_textOut(p.x, p.y, "Hello");

	// comment this scope to check whether is overlap
	{
		hdc.setMappingMode(PW5_MappingMode::LowEnglish);
		p = { 100, 100 };
		::DPtoLP(hdc, &p, 1);
		SGE_DUMP_VAR(p.x, p.y);
		hdc.Fmt_textOut(p.x, p.y, "Hello(overlap 2)");
	}
	// comment this scope to check whether is overlap
	{
		hdc.setMappingMode(PW5_MappingMode::HighEnglish);
		p = { 100, 100 };
		::DPtoLP(hdc, &p, 1);
		SGE_DUMP_VAR(p.x, p.y);
		hdc.Fmt_textOut(p.x, p.y, "Hello(overlap 3)");
	}
}

void PW5_MyMappingMode::_drawClientRectCoordinate(PW5_MappingMode v) {
	using SRC = PW5_MappingMode;

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

	if (v == PW5_MappingMode::None) {
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

}

#endif