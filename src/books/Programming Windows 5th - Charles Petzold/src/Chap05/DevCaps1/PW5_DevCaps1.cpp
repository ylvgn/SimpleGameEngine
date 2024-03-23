#if SGE_OS_WINDOWS

#include "PW5_DevCaps1.h"

namespace sge {

SGE_DEFINEMARK(MyDevcapsDM)

const DefineMark* MyDevcapsDM::s_getMark() {
	class DM : public DefineMark {
	public:
		static void s_eval(	int& v,
							const Item& item,
							const DefineMark::NativeUIWindow* obj)
		{
			auto* win = static_cast<const PW5_MyDefineMarkWindow*>(obj);
			v = ::GetDeviceCaps(win->_hdc, item.id);
		}

		DM() {
			static Item dm[] = {
				// 1 inch = 25.4 millimeters, 25.4 is constant for convert from inches to millimeters
				// HORZSIZE/VERTSIZE are fixed to indicate a standard monitor size, These values are the same regardless of what pixel dimension you choose
				SGE_DEFINEMARK__ITEM(HORZSIZE,      "Width in millimeters"),		// logical width  = 25.4 * horizontal resolution / logical pixel x
				SGE_DEFINEMARK__ITEM(VERTSIZE,      "Height in millimeters"),		// logical height = 25.4 * vertical resolution / logical pixel y
				// pixel dimension
				SGE_DEFINEMARK__ITEM(HORZRES,       "Width in pixels"),				// horizontal resolution in pixels, same as ::GetSystemMetrics(SM_CXSCREEN)
				SGE_DEFINEMARK__ITEM(VERTRES,       "Height in raster lines"),		// vertical resolution in pixels, same as ::GetSystemMetrics(SM_CYSCREEN)
				// Video memory can be organized either with consecutive color bits for each pixel or with each color bit in a separate color plane of memory
				SGE_DEFINEMARK__ITEM(BITSPIXEL,     "Color bits per pixel"),		// 32 bit color nowadays (r8g8b8a8)
				SGE_DEFINEMARK__ITEM(PLANES,        "Number of color planes"),
				SGE_DEFINEMARK__ITEM(NUMBRUSHES,    "Number of device brushes"),
				SGE_DEFINEMARK__ITEM(NUMPENS,       "Number of device pens"),
				SGE_DEFINEMARK__ITEM(NUMMARKERS,    "Number of device markers"),
				SGE_DEFINEMARK__ITEM(NUMFONTS,      "Number of device fonts"),
				// For high−color and full−color display resolutions NUMCOLORS often return -1
				// -1 is unreliable, so we can use this formular: NUMCOLORS =  1 << (number of color planes * color bits per pixel)
				SGE_DEFINEMARK__ITEM(NUMCOLORS,     "Number of device colors"),
				SGE_DEFINEMARK__ITEM(PDEVICESIZE,   "Size of device structure"),
				// ASPECTX/ASPECTY/ASPECTXY are related to the video dimensions,
				// relative width, height, and diagonal size of each pixel
				// ASPECTXY = square root of (squares(ASPECTX) + squares(ASPECTY))
				SGE_DEFINEMARK__ITEM(ASPECTX,       "Relative width of pixel"),
				SGE_DEFINEMARK__ITEM(ASPECTY,       "Relative height of pixel"),
				SGE_DEFINEMARK__ITEM(ASPECTXY,      "Relative diagonal of pixel"),
				// logical pixel, related to the font
				// As with Windows 98, typical values of LOGPIXELSX and LOGPIXELSY,
				// are 96(small font) and 120(large font) dots per inch,
				// depending on whether you select a small font or large font
				SGE_DEFINEMARK__ITEM(LOGPIXELSX,    "Horizontal dots per inch"),	// logical pixel x
				SGE_DEFINEMARK__ITEM(LOGPIXELSY,    "Vertical dots per inch"),		// logical pixel y
				SGE_DEFINEMARK__ITEM(SIZEPALETTE,   "Number of palette entries"),
				// 256−color video adapters use color palettes, 
				// In that case, GetDeviceCaps with the NUMCOLORS index returns the number of colors reserved by Windows, which will be 20.
				// The remaining 236 colors can be set by a Windows program using the palette manager. 256 - 20 = 236
				SGE_DEFINEMARK__ITEM(NUMRESERVED,   "Reserved palette entries"),
				SGE_DEFINEMARK__ITEM(COLORRES,      "Actual color resolution")
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