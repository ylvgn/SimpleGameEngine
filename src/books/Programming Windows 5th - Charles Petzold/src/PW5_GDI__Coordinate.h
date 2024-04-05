#pragma once

#include "PW5_Common.h"

#if SGE_OS_WINDOWS

namespace sge {

#define PW5_MappingMode_ENUM_LIST(E) \
	E(None,			= 0)				\
	/* fully constrained */ \
	E(Text,			= MM_TEXT)			\
	E(LowMetric,	= MM_LOMETRIC)		\
	E(HighMetric,	= MM_HIMETRIC)		\
	E(LowEnglish,	= MM_LOENGLISH)		\
	E(HighEnglish,	= MM_HIENGLISH)		\
	E(Twips,		= MM_TWIPS)			\
	/* partly constrained */ \
	E(Isotropic,	= MM_ISOTROPIC)		\
	/* unconstrained */ \
	E(Anisotropic,	= MM_ANISOTROPIC)	\
//----
SGE_ENUM_CLASS(PW5_MappingMode, u8)

} // namespace sge

namespace sge {
namespace GDI {

	// 25.4 mm ~= 1 inch
	static constexpr float kInchToMMUnitFactor = 25.4f;
	static constexpr float kMMToInchUnitFactor = 1.f / kInchToMMUnitFactor;

	// 1 point ~= 1/72 inch
	static constexpr float kInchToPointUnitFactor = 72.f;
	static constexpr float kPointToInchUnitFactor = 1.f / kInchToPointUnitFactor;

	// 1/20 point ~= 1/1440 inch
	static constexpr float kInchToTwipsUnitFactor = kInchToPointUnitFactor * 20.f;
	static constexpr float kTwipsToInchUnitFactor = 1.f / kInchToTwipsUnitFactor;

	// dpi dots == 1 inch
	inline Vec2f dotsPerInch(::HDC hdc) {
		return Vec2f {
			static_cast<float>(::GetDeviceCaps(hdc, LOGPIXELSX)),
			static_cast<float>(::GetDeviceCaps(hdc, LOGPIXELSY))
		};
	}

	inline float mmToInch(float mm)			{ return mm * kMMToInchUnitFactor; }
	inline Vec2f mmToInch(const Vec2f& mm)	{ return { mmToInch(mm.x), mmToInch(mm.y) }; }
	inline Vec2f mmToInch(const Vec2i& mm)  {
		auto f = Vec2f::s_cast(mm);
		return mmToInch(f);
	}
	inline void mmToInch(Vec2i& o, const Vec2i& mm) {
		auto f = mmToInch(mm);
		o.x = static_cast<int>(Math::round(f.x));
		o.y = static_cast<int>(Math::round(f.y));
	}

	inline float mmToPixel(float mm, float dpi) { return dpi * mm / kInchToMMUnitFactor; }
	inline Vec2f mmToPixel(const Vec2f& mm, const Vec2f& dpi) {
		return { mmToPixel(mm.x, dpi.x), mmToPixel(mm.y, dpi.y) };
	}
	inline Vec2f mmToPixel(const Vec2i& mm_, const Vec2i& dpi_) {
		auto mm = Vec2f::s_cast(mm_);
		auto dpi = Vec2f::s_cast(dpi_);
		return mmToPixel(mm, dpi);
	}
	inline Vec2f mmToPixel(::HDC hdc, const Vec2f& mm) {
		Vec2f dpi = dotsPerInch(hdc);
		return mmToPixel(mm, dpi);
	}
	inline void mmToPixel(Vec2i& o, ::HDC hdc, const Vec2f& mm) {
		auto f = mmToPixel(hdc, mm);
		o.x = static_cast<int>(Math::round(f.x));
		o.y = static_cast<int>(Math::round(f.y));
	}

	inline float inchToTwips(float inch) { return inch * kInchToTwipsUnitFactor; }
	inline Vec2f inchToTwips(const Vec2f& inch) { return { inchToTwips(inch.x), inchToTwips(inch.y) }; }
	inline Vec2f inchToTwips(const Vec2i& inch) {
		auto f = Vec2f::s_cast(inch);
		return inchToTwips(f);
	}

	inline float inchToMM(float inch) { return inch * kInchToMMUnitFactor; }
	inline Vec2f inchToMM(const Vec2f& inch) { return { inchToMM(inch.x), inchToMM(inch.y) }; }
	inline Vec2f inchToMM(const Vec2i& inch) {
		auto f = Vec2f::s_cast(inch);
		return inchToMM(f);
	}
	inline void inchToMM(Vec2i& o, const Vec2i& inch) {
		auto f = inchToMM(inch);
		o.x = static_cast<int>(Math::round(f.x));
		o.y = static_cast<int>(Math::round(f.y));
	}

	inline float mmToTwips(float mm)		{ return inchToTwips(mmToInch(mm)); }
	inline Vec2f mmToTwips(const Vec2f& mm) { return { mmToTwips(mm.x), mmToTwips(mm.y) }; }
	inline Vec2f mmToTwips(const Vec2i& mm) {
		auto f = Vec2f::s_cast(mm);
		return mmToTwips(f);
	}

		inline bool dPtoLP(const ::HDC& hdc, ::RECT& rect) {
		return ::DPtoLP(hdc, reinterpret_cast<::LPPOINT>(&rect), 2);
	}
	inline bool dPtoLP(const ::HDC& hdc, ::POINT& pt) {
		return ::DPtoLP(hdc, &pt, 1);
	}
	inline bool dPtoLP(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		Win32Util::convert(pt_, pt);
		bool ok = ::DPtoLP(hdc, &pt_, 1);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	inline bool lPtoDP(const ::HDC& hdc, ::RECT& rect) {
		return ::LPtoDP(hdc, reinterpret_cast<::LPPOINT>(&rect), 2);
	}
	inline bool lPtoDP(const ::HDC& hdc, ::POINT& pt) {
		return ::LPtoDP(hdc, &pt, 1);
	}
	inline bool lPtoDP(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		Win32Util::convert(pt_, pt);
		bool ok = ::LPtoDP(hdc, &pt_, 1);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	// device coordinates origin set/get
	inline bool setViewportOrg(const ::HDC& hdc, const ::POINT& pt) {
		return ::SetViewportOrgEx(hdc, pt.x, pt.y, nullptr);
	}
	inline bool setViewportOrg(const ::HDC& hdc, const Vec2f& pt) {
		return ::SetViewportOrgEx(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), nullptr);
	}
	inline bool setViewportOrg(const ::HDC& hdc, int x, int y) {
		return ::SetViewportOrgEx(hdc, x, y, nullptr);
	}

	inline bool getViewportOrg(const ::HDC& hdc, ::POINT& pt) {
		return ::GetViewportOrgEx(hdc, &pt);
	}
	inline bool getViewportOrg(const ::HDC& hdc, Vec2i& pt) {
		::POINT pt_;
		bool ok = ::GetViewportOrgEx(hdc, &pt_);
		Win32Util::convert(pt, pt_);
		return ok;
	}
	inline bool getViewportOrg(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		bool ok = ::GetViewportOrgEx(hdc, &pt_);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	// logical coordinates origin set/get
	inline bool setWindowOrg(const ::HDC& hdc, const ::POINT& pt) {
		return ::SetWindowOrgEx(hdc, pt.x, pt.y, nullptr);
	}
	inline bool setWindowOrg(const ::HDC& hdc, const Vec2f& pt) {
		return ::SetWindowOrgEx(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), nullptr);
	}
	inline bool setWindowOrg(const ::HDC& hdc, int x, int y) {
		return ::SetWindowOrgEx(hdc, x, y, nullptr);
	}

	inline bool getWindowOrg(const ::HDC& hdc, ::POINT& pt) {
		return ::GetWindowOrgEx(hdc, &pt);
	}
	inline bool getWindowOrg(const ::HDC& hdc, Vec2i& pt) {
		::POINT pt_;
		bool ok = ::GetWindowOrgEx(hdc, &pt_);
		Win32Util::convert(pt, pt_);
		return ok;
	}
	inline bool getWindowOrg(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		bool ok = ::GetWindowOrgEx(hdc, &pt_);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	inline bool setViewportExt(const ::HDC& hdc, int x, int y) {
		return ::SetViewportExtEx(hdc, x, y, nullptr);
	}
	inline bool setViewportExt(const ::HDC& hdc, const ::SIZE& sz) {
		return ::SetViewportExtEx(hdc, sz.cx, sz.cy, nullptr);
	}
	inline bool setViewportExt(const ::HDC& hdc, const Vec2f& sz) {
		return ::SetViewportExtEx(hdc, static_cast<int>(sz.x), static_cast<int>(sz.y), nullptr);
	}

	inline bool setWindowExt(const ::HDC& hdc, int x, int y) {
		return ::SetWindowExtEx(hdc, x, y, nullptr);
	}
	inline bool setWindowExt(const ::HDC& hdc, const ::SIZE& sz) {
		return ::SetWindowExtEx(hdc, sz.cx, sz.cy, nullptr);
	}
	inline bool setWindowExt(const ::HDC& hdc, const Vec2f& sz) {
		return ::SetWindowExtEx(hdc, static_cast<int>(sz.x), static_cast<int>(sz.y), nullptr);
	}

	inline bool getViewportExt(const ::HDC& hdc, ::SIZE& sz) { return ::GetViewportExtEx(hdc, &sz); }
	inline bool getViewportExt(const ::HDC& hdc, Vec2i& sz) {
		::SIZE sz_;
		bool ok = ::GetViewportExtEx(hdc, &sz_);
		Win32Util::convert(sz, sz_);
		return ok;
	}
	inline bool getViewportExt(const ::HDC& hdc, Vec2f& sz) {
		::SIZE sz_;
		bool ok = ::GetViewportExtEx(hdc, &sz_);
		Win32Util::convert(sz, sz_);
		return ok;
	}

	inline bool getWindowExt(const ::HDC& hdc, ::SIZE& sz) { return ::GetWindowExtEx(hdc, &sz); }
	inline bool getWindowExt(const ::HDC& hdc, Vec2i& sz) {
		::SIZE sz_;
		bool ok = ::GetWindowExtEx(hdc, &sz_);
		Win32Util::convert(sz, sz_);
		return ok;
	}
	inline bool getWindowExt(const ::HDC& hdc, Vec2f& sz) {
		::SIZE sz_;
		bool ok = ::GetWindowExtEx(hdc, &sz_);
		Win32Util::convert(sz, sz_);
		return ok;
	}

	inline float getDisplayScaleRatio() {
		::POINT ptZero = { 0, 0 };
		::HMONITOR hMonitor = ::MonitorFromPoint(ptZero, MONITOR_DEFAULTTONULL);

		::MONITORINFOEX monitorInfo;
		g_bzero(monitorInfo);
		monitorInfo.cbSize = sizeof(monitorInfo);
		::GetMonitorInfo(hMonitor, &monitorInfo);

		auto displayResolutionScaledWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		if (displayResolutionScaledWidth == 0) return 1.f; // fallback

		::DEVMODE devmode;
		g_bzero(devmode);
		devmode.dmSize = sizeof(devmode);
		::EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devmode);

		auto displayResolutionWidth = devmode.dmPelsWidth;
		if (displayResolutionWidth == 0) return 1.f; // fallback
		return static_cast<float>(displayResolutionWidth) / displayResolutionScaledWidth;
	}

} // namespace GDI
} // namespace sge

#endif