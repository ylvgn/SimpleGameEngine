#pragma once

#if SGE_OS_WINDOWS

namespace sge {

#define PW5_StockLogicalObject_Brush_ENUM_LIST(E) \
	E(None,		= NULL_BRUSH)	\
	E(White,	= WHITE_BRUSH)	\
	E(LtGray,	= LTGRAY_BRUSH)	\
	E(Gray,		= GRAY_BRUSH)	\
	E(DkGray,	= DKGRAY_BRUSH)	\
	E(Black,	= BLACK_BRUSH)	\
	E(Hollow,	= HOLLOW_BRUSH)	\
//----
SGE_ENUM_DECLARE(PW5_StockLogicalObject_Brush, u8)
SGE_ENUM_ALL_OPERATOR(PW5_StockLogicalObject_Brush)

// The stock pens are 1 pixel wide default
#define PW5_StockLogicalObject_Pen_ENUM_LIST(E) \
	E(None,		= NULL_PEN)	/* NULL_PEN is a pen that doesn't draw */	\
	E(White,	= WHITE_PEN) \
	E(Black,	= BLACK_PEN) /*default pen*/ \
//----
SGE_ENUM_CLASS(PW5_StockLogicalObject_Pen, u8)

#define PW5_StockLogicalObject_Font_ENUM_LIST(E) \
	E(OemFixed,		= OEM_FIXED_FONT) \
	E(AnsiFixed,	= ANSI_FIXED_FONT) \
	E(AnsiVar,		= ANSI_VAR_FONT) \
	E(System,		= SYSTEM_FONT) \
	E(DeviceDefault,= DEVICE_DEFAULT_FONT) \
	E(SystemFixed,	= SYSTEM_FIXED_FONT) \
//----
SGE_ENUM_CLASS(PW5_StockLogicalObject_Font, u8)

enum class PW5_StockLogicalObject : u8;
constexpr PW5_StockLogicalObject PW5_StockLogicalObject_make(PW5_StockLogicalObject_Brush v) {
	return static_cast<PW5_StockLogicalObject>(enumInt(v));
}
constexpr PW5_StockLogicalObject PW5_StockLogicalObject_make(PW5_StockLogicalObject_Pen v) {
	return static_cast<PW5_StockLogicalObject>(enumInt(v));
}
constexpr PW5_StockLogicalObject PW5_StockLogicalObject_make(PW5_StockLogicalObject_Font v) {
	return static_cast<PW5_StockLogicalObject>(enumInt(v));
}
#define PW5_StockLogicalObject_ENUM_LIST(E) \
	E(WhiteBrush,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Brush::White)) \
	E(LtGrayBrush,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Brush::LtGray)) \
	E(GrayBrush,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Brush::Gray)) \
	E(DkGrayBrush,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Brush::DkGray)) \
	E(BlackBrush,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Brush::Black)) \
	E(HollowBrush,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Brush::Hollow)) \
	\
	E(NonePen,			= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Pen::None)) \
	E(WhitePen,			= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Pen::White)) \
	E(BlackPen,			= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Pen::Black)) \
	\
	E(OemFixedFont,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Font::OemFixed)) \
	E(AnsiFixedFont,	= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Font::AnsiFixed)) \
	E(AnsiVarFont,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Font::AnsiVar)) \
	E(SystemFont,		= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Font::System)) \
	E(DeviceDefaultFont,= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Font::DeviceDefault)) \
	E(SystemFixedFont,	= PW5_StockLogicalObject_make(PW5_StockLogicalObject_Font::SystemFixed)) \
//----
SGE_ENUM_DECLARE(PW5_StockLogicalObject, u8)
SGE_ENUM_ALL_OPERATOR(PW5_StockLogicalObject)

} // namespace sge

namespace sge {
namespace GDI {

	using StockObj			= PW5_StockLogicalObject;
	using StockObj_Brush	= PW5_StockLogicalObject_Brush;
	using StockObj_Pen		= PW5_StockLogicalObject_Pen;
	using StockObj_Font		= PW5_StockLogicalObject_Font;

	inline ::HGDIOBJ getStockObject(StockObj flag) {
		return ::GetStockObject(enumInt(flag));
	}
	inline ::HBRUSH getStockObject(StockObj_Brush flag) {
		return GetStockBrush(enumInt(flag));
	}
	inline ::HPEN getStockObject(StockObj_Pen flag) {
		return GetStockPen(enumInt(flag));
	}
	inline ::HFONT getStockObject(StockObj_Font flag) {
		return GetStockFont(enumInt(flag));
	}

} // namespace GDI
} // namespace sge

#endif