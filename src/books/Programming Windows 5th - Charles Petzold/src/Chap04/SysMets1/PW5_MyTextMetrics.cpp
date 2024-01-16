#include "PW5_MyTextMetrics.h"

#if SGE_OS_WINDOWS

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
	ScopedHDC hdc(_hwnd);
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
	hdc.textOutf(300, 300, "cxChar={}, cyChar={}, cxCaps={}", cxChar, cyChar, cxCaps);
}

}

#endif // SGE_OS_WINDOWS