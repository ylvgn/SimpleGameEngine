#pragma once

#if SGE_OS_WINDOWS

namespace sge {

#define PW5_MapMode_ENUM_LIST(E)	\
	E(None,			= 0)				\
	/* fully constrained */				\
	E(Text,			= MM_TEXT)			\
	E(LowMetric,	= MM_LOMETRIC)		\
	E(HighMetric,	= MM_HIMETRIC)		\
	E(LowEnglish,	= MM_LOENGLISH)		\
	E(HighEnglish,	= MM_HIENGLISH)		\
	E(Twips,		= MM_TWIPS)			\
	/* partly constrained */			\
	E(Isotropic,	= MM_ISOTROPIC)		\
	/* unconstrained */					\
	E(Anisotropic,	= MM_ANISOTROPIC)	\
//----
SGE_ENUM_CLASS(PW5_MapMode, u8)

#define PW5_CoordinateDir_ENUM_LIST(E)	\
	E(RD,) \
	E(RU,) \
	E(LD,) \
	E(LU,) \
//----
SGE_ENUM_CLASS(PW5_CoordinateDir, u8)

#define PW5_Dir_ENUM_LIST(E) \
	E(None,  = 0) \
	E(Left,  = 1 << 0) \
	E(Down,  = 1 << 1) \
	E(Right, = 1 << 2) \
	E(Up,	 = 1 << 3) \
//----
SGE_ENUM_CLASS(PW5_Dir, u8)
SGE_ENUM_ALL_OPERATOR(PW5_Dir)

inline PW5_CoordinateDir PW5_CoordinateDir_make(PW5_Dir dir) {
	using CoordinateDir = PW5_CoordinateDir;
	using Dir = PW5_Dir;

	if (BitUtil::hasAny(dir, Dir::Right))
		return BitUtil::hasAny(dir, Dir::Down)
			? CoordinateDir::RD
			: CoordinateDir::RU;
	else
		return BitUtil::hasAny(dir, Dir::Down)
			? CoordinateDir::LD
			: CoordinateDir::LU;
}

} // namespace sge


namespace sge {
namespace GDI {

	using MapMode		= PW5_MapMode;
	using CoordinateDir = PW5_CoordinateDir;

	// 25.4 mm ~= 1 inch
	static constexpr float kInchToMMUnitFactor = 25.4f;
	static constexpr float kMMToInchUnitFactor = 1.f / kInchToMMUnitFactor;

	// a 72-point font is rendered as 96 pixels when 96 dots per inch (DPI)
	static constexpr float kPointToPixelUnitFactor = 96.f / 72.f;
	static constexpr float kPixelToPointUnitFactor = 1.f / kPointToPixelUnitFactor;

	// 1 point ~= 1/72 inch
	static constexpr float kInchToPointUnitFactor = 72.f;
	static constexpr float kPointToInchUnitFactor = 1.f / kInchToPointUnitFactor;

	// 1/20 point ~= 1/1440 inch
	static constexpr float kInchToTwipsUnitFactor = kInchToPointUnitFactor * 20.f;
	static constexpr float kTwipsToInchUnitFactor = 1.f / kInchToTwipsUnitFactor;

	//100% (96 DPI), 125% (120 DPI), and 150% (144 DPI)
	static constexpr float kDIPToDPIUnitFactor = USER_DEFAULT_SCREEN_DPI;
	static constexpr float kDPIToDIPUnitFactor = 1 / kDIPToDPIUnitFactor;

	inline Vec2f dpi(::HDC hdc) {
		return Vec2f {
			static_cast<float>(::GetDeviceCaps(hdc, LOGPIXELSX)),
			static_cast<float>(::GetDeviceCaps(hdc, LOGPIXELSY))
		};
	}

	// A DIP is defined as 1/96th of a logical inch
	inline Vec2f dpiScale(::HDC hdc) {
		return dpi(hdc) * kDPIToDIPUnitFactor;
	}
	inline Vec2f pointToPixelFactor(::HDC hdc) {
		return dpiScale(hdc) * kPointToPixelUnitFactor;
	}
	inline Vec2f pixelToDIP(::HDC hdc, const Vec2f& pixels) {
		return pixels / dpiScale(hdc);
	}

	inline float mmToInch(float mm)	{ return mm * kMMToInchUnitFactor; }
	inline Vec2f mmToInch(const Vec2f& mm) {
		return {
			mmToInch(mm.x),
			mmToInch(mm.y)
		};
	}
	inline Vec2f mmToInch(const Vec2i& mm) {
		auto f = Vec2f::s_cast(mm);
		return mmToInch(f);
	}
	inline void mmToInch(Vec2i& o, const Vec2i& mm) {
		auto f = mmToInch(mm);
		o.x = static_cast<int>(Math::round(f.x));
		o.y = static_cast<int>(Math::round(f.y));
	}

	inline Vec2f inchToPoint(const Vec2f& inch) {
		return inch * kInchToPointUnitFactor;
	}
	inline Vec2f pointToInch(const Vec2f& point) {
		return point * kPointToInchUnitFactor;
	}

	inline Vec2f pointToPixel(::HDC hdc, const Vec2f& point) {
		return point * pointToPixelFactor(hdc);
	}
	inline Vec2f mmToPixel(::HDC hdc, const Vec2f& mm) {
		return pointToPixel(hdc, inchToPoint(mmToInch(mm)));
	}
	inline void mmToPixel(Vec2i& o, ::HDC hdc, const Vec2f& mm) {
		auto f = mmToPixel(hdc, mm);
		o.x = static_cast<int>(Math::round(f.x));
		o.y = static_cast<int>(Math::round(f.y));
	}
	inline Vec2f mmToPixel(const Vec2f& mm, const Vec2f& dpi) {
		return inchToPoint(mmToInch(mm)) * dpi * kPointToPixelUnitFactor * kDPIToDIPUnitFactor;
	}
	inline void mmToPixel(Vec2i& o, const Vec2f& dpi, const Vec2f& mm) {
		auto f = mmToPixel(dpi, mm);
		o.x = static_cast<int>(Math::round(f.x));
		o.y = static_cast<int>(Math::round(f.y));
	}
	inline float inchToTwips(float inch) { return inch * kInchToTwipsUnitFactor; }
	inline Vec2f inchToTwips(const Vec2f& inch) {
		return {
			inchToTwips(inch.x),
			inchToTwips(inch.y)
		};
	}
	inline Vec2f inchToTwips(const Vec2i& inch) {
		auto f = Vec2f::s_cast(inch);
		return inchToTwips(f);
	}

	inline float inchToMM(float inch) { return inch * kInchToMMUnitFactor; }
	inline Vec2f inchToMM(const Vec2f& inch) {
		return {
			inchToMM(inch.x),
			inchToMM(inch.y)
		};
	}
	inline Vec2f inchToMM(const Vec2i& inch) {
		auto f = Vec2f::s_cast(inch);
		return inchToMM(f);
	}
	inline void inchToMM(Vec2i& o, const Vec2i& inch) {
		auto f = inchToMM(inch);
		o.x = static_cast<int>(Math::round(f.x));
		o.y = static_cast<int>(Math::round(f.y));
	}

	inline float mmToTwips(float mm) { return inchToTwips(mmToInch(mm)); }
	inline Vec2f mmToTwips(const Vec2f& mm) {
		return {
			mmToTwips(mm.x),
			mmToTwips(mm.y)
		};
	}
	inline Vec2f mmToTwips(const Vec2i& mm) {
		auto f = Vec2f::s_cast(mm);
		return mmToTwips(f);
	}

	inline auto setMapMode(::HDC hdc, MapMode flag) {
		return ::SetMapMode(hdc, enumInt(flag));
	}
	MapMode getMapMode(::HDC hdc) {
		return static_cast<MapMode>(::GetMapMode(hdc));
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

	inline bool setViewportOrg(const ::HDC& hdc, const ::POINT& pt) {
		return ::SetViewportOrgEx(hdc, pt.x, pt.y, nullptr); // suppose left-top is 0,0
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
	inline bool setViewportExt(const ::HDC& hdc, const Vec2i& sz) {
		return ::SetViewportExtEx(hdc, sz.x, sz.y, nullptr);
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
		monitorInfo.cbSize = sizeof(monitorInfo);
		::GetMonitorInfo(hMonitor, &monitorInfo);

		auto displayResolutionScaledWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		if (displayResolutionScaledWidth == 0) return 1.f; // fallback

		::DEVMODE devmode;
		devmode.dmSize = sizeof(devmode);
		::EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devmode);

		auto displayResolutionWidth = devmode.dmPelsWidth;
		if (displayResolutionWidth == 0) return 1.f; // fallback
		return static_cast<float>(displayResolutionWidth) / displayResolutionScaledWidth;
	}

	inline PW5_CoordinateDir getCoordViewportDir(::HDC hdc) {
		using CoordinateDir = PW5_CoordinateDir;
		using Dir = PW5_Dir;

		::POINT pt { 0, 0 };
		GDI::dPtoLP(hdc, pt);

		::POINT o;
		GDI::getWindowOrg(hdc, o);

		Dir dir = Dir::None;

		if (o.x - pt.x >= 0) dir |= Dir::Right;
		else dir |= Dir::Left;

		if (o.y - pt.y >= 0) dir |= Dir::Down;
		else dir |= Dir::Up;

		return PW5_CoordinateDir_make(dir);
	}

	inline PW5_CoordinateDir getCoordWindowDir(::HDC hdc) {
		using CoordinateDir = PW5_CoordinateDir;
		using Dir = PW5_Dir;

		::POINT pt { 0, 0 };
		GDI::lPtoDP(hdc, pt);

		::POINT o;
		GDI::getViewportOrg(hdc, o);

		Dir dir = Dir::None;

		if (o.x - pt.x >= 0) dir |= Dir::Right;
		else dir |= Dir::Left;

		if (o.y - pt.y >= 0) dir |= Dir::Down;
		else dir |= Dir::Up;

		return PW5_CoordinateDir_make(dir);
	}

	inline PW5_CoordinateDir getCoordinateDir(::HDC hdc, bool isViewportSpace) {
		if (isViewportSpace) return getCoordViewportDir(hdc);
		else return getCoordWindowDir(hdc);
	}

	inline void translateLTOffset(::HDC hdc, Vec2f& o, const Vec2f& offset, bool isViewportSpace) {
		using SRC = PW5_CoordinateDir;
		auto d = getCoordinateDir(hdc, isViewportSpace);
		switch (d)
		{
			case SRC::RD: o += offset; break;
			case SRC::RU: o.x += offset.x; o.y -= offset.y; break;
			case SRC::LD: o.x -= offset.x; o.y += offset.y; break;
			case SRC::LU: o -= offset; break;
		}
	}

} // namespace GDI
} // namespace sge

#endif