#if SGE_OS_WINDOWS

#include "PW5_MyTextMetrics.h"

namespace sge {
/*
	The term "leading" refers to space that a printer inserts between lines of text.

	typedef struct tagTEXTMETRIC
	{
		LONG tmHeight;  // tmHeight = tmAscent + tmDescent. represent the maximum vertical
		LONG tmAscent;  // above the base line
		LONG tmDescent; // below the base line
		LONG tmInternalLeading; // internal leading is included in tmAscent and is often the space in which accent marks appear. like spanish
		LONG tmExternalLeading; // This is an amount of space that the designer of the font suggests be added between successive rows of displayed text.
		LONG tmAveCharWidth;	// a weighted average of lowercase characters
		LONG tmMaxCharWidth;	// the width of the widest character in the font
			[other structure fields]
	}
	TEXTMETRIC, * PTEXTMETRIC;
*/

void PW5_MyTextMetrics::onDraw() {
	//_example1();
	_example2();
}

void PW5_MyTextMetrics::_example1() {
	ScopedGetDC hdc(_hwnd);

	::TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	int cxChar = tm.tmAveCharWidth;
	int cyChar = tm.tmHeight + tm.tmExternalLeading; // tmExternalLeading is 0
	int cxCaps = cxChar; // an average width of uppercase letters
	// The low bit of the tmPitchAndFamily field in the TEXTMETRIC structure
		// is 1 for a variable−width font
		// and 0 for a fixed−pitch font
	if (tm.tmPitchAndFamily & 1) {
		// variable−width font
		// For a variable−width font, cxCaps is set to 150 percent of cxChar.
		cxCaps = static_cast<int>(1.5f * cxChar);
	}
	hdc.Fmt_textOut(300, 300, "cxChar={}, cyChar={}, cxCaps={}", cxChar, cyChar, cxCaps);
}

void PW5_MyTextMetrics::_example2() {
	ScopedGetDC hdc(_hwnd);
	_tm.create(hdc);

	{ // MM_TEXT
		hdc.setMapMode(PW5_MapMode::None);
		int spacingY = _tm.height;
		int x = 300;
		int y = 100;

		hdc.Fmt_textOut(x, y, "(MM_TEXT)height={}", _tm.height); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)ascent={}", _tm.ascent); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)descent={}", _tm.descent); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)internalLeading={}", _tm.internalLeading); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)maxCharWidth={}", _tm.maxCharWidth); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)externalLeading={}", _tm.externalLeading); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)aveCharWidth={}", _tm.aveCharWidth); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)aveCharHeight={}", _tm.aveCharHeight); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)aveCharWidthUpperCase={}", _tm.aveCharWidthUpperCase); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_TEXT)isFixedPitch={}", static_cast<int>(_tm.isFixedPitch)); y += spacingY;
	}

	{ // MM_LOENGLISH
		// when you call GetTextMetrics for information about the heightand width of characters,
		// the mapping mode should be set to the same mapping mode that
		// you'll be using when you draw text based on these sizes

		hdc.setMapMode(PW5_MapMode::LowEnglish);
		Vec2f pt { 300,100 };
		hdc.dPtoLP(pt);
		pt.x += 200;

		int x = static_cast<int>(pt.x);
		int y = static_cast<int>(pt.y);

		int spacingY = -_tm.height;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)height={}", _tm.height); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)ascent={}", _tm.ascent); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)descent={}", _tm.descent); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)internalLeading={}", _tm.internalLeading); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)maxCharWidth={}", _tm.maxCharWidth); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)externalLeading={}", _tm.externalLeading); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)aveCharWidth={}", _tm.aveCharWidth); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)aveCharHeight={}", _tm.aveCharHeight); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)aveCharWidthUpperCase={}", _tm.aveCharWidthUpperCase); y += spacingY;
		hdc.Fmt_textOut(x, y, "(MM_LOENGLISH)isFixedPitch={}", static_cast<int>(_tm.isFixedPitch)); y += spacingY;
	}
}

}

#endif // SGE_OS_WINDOWS