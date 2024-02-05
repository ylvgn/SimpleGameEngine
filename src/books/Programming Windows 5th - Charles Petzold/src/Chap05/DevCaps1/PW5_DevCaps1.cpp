#if SGE_OS_WINDOWS

#include "PW5_DevCaps1.h"

namespace sge {

SGE_DEFINE_MARK_STRUCT(MyDevcapsDM)

template<>
const DefineMarkInfo* DefineMarkOf<MyDevcapsDM>() {
	class DM : public DefineMarkInfo {
	public:
		DM() {
			static DefineMark dm[] = {
				SGE_DEFINE_MARK__ITEM(HORZSIZE,      Width in millimeters),
				SGE_DEFINE_MARK__ITEM(VERTSIZE,      Height in millimeters),
				SGE_DEFINE_MARK__ITEM(HORZRES,       Width in pixels),
				SGE_DEFINE_MARK__ITEM(VERTRES,       Height in raster lines),
				SGE_DEFINE_MARK__ITEM(BITSPIXEL,     Color bits per pixel),
				SGE_DEFINE_MARK__ITEM(PLANES,        Number of color planes),
				SGE_DEFINE_MARK__ITEM(NUMBRUSHES,    Number of device brushes),
				SGE_DEFINE_MARK__ITEM(NUMPENS,       Number of device pens),
				SGE_DEFINE_MARK__ITEM(NUMMARKERS,    Number of device markers),
				SGE_DEFINE_MARK__ITEM(NUMFONTS,      Number of device fonts),
				SGE_DEFINE_MARK__ITEM(NUMCOLORS,     Number of device colors),
				SGE_DEFINE_MARK__ITEM(PDEVICESIZE,   Size of device structure),
				SGE_DEFINE_MARK__ITEM(ASPECTX,       Relative width of pixel),
				SGE_DEFINE_MARK__ITEM(ASPECTY,       Relative height of pixel),
				SGE_DEFINE_MARK__ITEM(ASPECTXY,      Relative diagonal of pixel),
				SGE_DEFINE_MARK__ITEM(LOGPIXELSX,    Horizontal dots per inch),
				SGE_DEFINE_MARK__ITEM(LOGPIXELSY,    Vertical dots per inch),
				SGE_DEFINE_MARK__ITEM(SIZEPALETTE,   Number of palette entries),
				SGE_DEFINE_MARK__ITEM(NUMRESERVED,   Reserved palette entries),
				SGE_DEFINE_MARK__ITEM(COLORRES,      Actual color resolution)
			};
			setDefineMarks(dm);
		}
	};

	static DM info;
	return &info;
}


void PW5_DevCaps1::onCreate(CreateDesc& desc) {
	_dmInfo = MyDevcapsDM::s_getMarkOf();
	Base::onCreate(desc);
}

}

#endif