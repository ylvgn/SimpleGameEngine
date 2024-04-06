#pragma once

#if SGE_OS_WINDOWS

namespace sge {
namespace GDI {

	static constexpr Color4f kWhite		{ 1,1,1,1 };
	static constexpr Color4f kBlack		{ 0,0,0,1 };
	static constexpr Color4f kRed		{ 1,0,0,1 };
	static constexpr Color4f kGreen		{ 0,1,0,1 };
	static constexpr Color4f kBlue		{ 0,0,1,1 };
	static constexpr Color4f kYellow	{ 1,1,0,1 };
	static constexpr Color4f kViolet	{ 1,0,1,1 };
	static constexpr Color4f kCyan		{ 0,1,1,1 };
	static constexpr Color4f kGray		{ 0.2f,0.2f,0.2f,1.f };

	static constexpr Color4b kbWhite	{ 255, 255, 255, 255 };
	static constexpr Color4b kbBlack	{ 0,   0,   0,   255 };
	static constexpr Color4b kbRed		{ 255, 0,   0,   255 };
	static constexpr Color4b kbGreen	{ 0,   255, 0,   255 };
	static constexpr Color4b kbBlue		{ 0,   0,   255, 255 };
	static constexpr Color4b kbYellow	{ 255, 255, 0,   255 };
	static constexpr Color4b kbViolet	{ 255, 0,   255, 255 };
	static constexpr Color4b kbCyan		{ 0,   255, 255, 255 };
	static constexpr Color4b kbGray		{ 51,  51,  51,  255 };

	inline ::COLORREF COLORREF_make(const Color4b& c)		{ return RGB(c.r, c.g, c.b); }
	inline ::COLORREF COLORREF_make(const Color4f& c)		{ return RGB(c.r*255, c.g*255, c.b*255); }

} // namespace GDI
} // namespace sge

#endif