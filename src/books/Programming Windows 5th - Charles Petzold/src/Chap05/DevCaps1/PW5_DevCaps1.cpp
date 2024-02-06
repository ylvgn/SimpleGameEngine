#if SGE_OS_WINDOWS

#include "PW5_DevCaps1.h"

namespace sge {

SGE_DEFINEMARK(MyDevcapsDM)

const DefineMark* MyDevcapsDM::s_getMark() {
	class DM : public DefineMark {
	public:
		static void s_eval(int& v, const PW5_MyDefineMarkWindow* obj, const Item& item) {
			v = ::GetDeviceCaps(GetDC(obj->hwnd()), item.id);
		}

		DM() {
			static Item dm[] = {
				SGE_DEFINEMARK__ITEM(HORZSIZE,      L"Width in millimeters"),		// logical width = 25.4 * 1920
				SGE_DEFINEMARK__ITEM(VERTSIZE,      L"Height in millimeters"),		// logical height
				SGE_DEFINEMARK__ITEM(HORZRES,       L"Width in pixels"),			// same as ::GetSystemMetrics(SM_CXSCREEN)
				SGE_DEFINEMARK__ITEM(VERTRES,       L"Height in raster lines"),		// same as ::GetSystemMetrics(SM_CYSCREEN)
				SGE_DEFINEMARK__ITEM(BITSPIXEL,     L"Color bits per pixel"),
				SGE_DEFINEMARK__ITEM(PLANES,        L"Number of color planes"),
				SGE_DEFINEMARK__ITEM(NUMBRUSHES,    L"Number of device brushes"),
				SGE_DEFINEMARK__ITEM(NUMPENS,       L"Number of device pens"),
				SGE_DEFINEMARK__ITEM(NUMMARKERS,    L"Number of device markers"),
				SGE_DEFINEMARK__ITEM(NUMFONTS,      L"Number of device fonts"),
				SGE_DEFINEMARK__ITEM(NUMCOLORS,     L"Number of device colors"),
				SGE_DEFINEMARK__ITEM(PDEVICESIZE,   L"Size of device structure"),
				SGE_DEFINEMARK__ITEM(ASPECTX,       L"Relative width of pixel"),
				SGE_DEFINEMARK__ITEM(ASPECTY,       L"Relative height of pixel"),
				SGE_DEFINEMARK__ITEM(ASPECTXY,      L"Relative diagonal of pixel"),
				SGE_DEFINEMARK__ITEM(LOGPIXELSX,    L"Horizontal dots per inch"),	// logical pixel x, 1 inch = 25.4 mm
				SGE_DEFINEMARK__ITEM(LOGPIXELSY,    L"Vertical dots per inch"),		// logical pixel y
				SGE_DEFINEMARK__ITEM(SIZEPALETTE,   L"Number of palette entries"),
				SGE_DEFINEMARK__ITEM(NUMRESERVED,   L"Reserved palette entries"),
				SGE_DEFINEMARK__ITEM(COLORRES,      L"Actual color resolution")
			};
			set(dm, &s_eval);
		}
	};

	static DM info;
	return &info;
}


void PW5_DevCaps1::onCreate(CreateDesc& desc) {
	_dm = MyDevcapsDM::s_getMark();
	Base::onCreate(desc);
}

}

#endif