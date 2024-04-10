#if SGE_OS_WINDOWS

#include "PW5_DevCaps1.h"

namespace sge {

PW5_MYDEFINATIONREMARKS(MyDevcapsDM)

const PW5_MyDefinationRemarks* MyDevcapsDM::s_getRemarks() {
	class DM : public PW5_MyDefinationRemarks {
		using Base = PW5_MyDefinationRemarks;
		using Item = Base::Item;
		static void s_eval(	int& v, const Item& item,
							const Base::NativeUIWindow* obj) {
			auto* win = static_cast<const PW5_MyDefineMarkWindow*>(obj);
			v = ::GetDeviceCaps(win->_hdc, item.id);
		}

	public:
		DM() {
			static Item dm[] = {
				// These values(physical screen in mm) are the same regardless of what pixel dimension you choose
				PW5_MYDEFINATIONREMARKS__ITEM(HORZSIZE,      "Width in millimeters of the physical screen"),
				PW5_MYDEFINATIONREMARKS__ITEM(VERTSIZE,      "Height in millimeters of the physical screen"),
				// pixel dimension (display resolution with desktop scaling)
					// horizontal resolution in pixels, kind of ::GetSystemMetrics(SM_CXSCREEN) without desktop scaling
					// vertical resolution in pixels, kind of ::GetSystemMetrics(SM_CYSCREEN) without desktop scaling
				PW5_MYDEFINATIONREMARKS__ITEM(HORZRES,       "Width in pixels of the display screen"), 
				PW5_MYDEFINATIONREMARKS__ITEM(VERTRES,       "Height iin pixels of the display screen"), 
				// Video memory can be organized either with consecutive color bits for each pixel or with each color bit in a separate color plane of memory
				PW5_MYDEFINATIONREMARKS__ITEM(BITSPIXEL,     "Color bits per pixel"),		// 32 bit color nowadays (r8g8b8a8)
				PW5_MYDEFINATIONREMARKS__ITEM(PLANES,        "Number of color planes"),
				PW5_MYDEFINATIONREMARKS__ITEM(NUMBRUSHES,    "Number of device brushes"),
				PW5_MYDEFINATIONREMARKS__ITEM(NUMPENS,       "Number of device pens"),
				PW5_MYDEFINATIONREMARKS__ITEM(NUMMARKERS,    "Number of device markers"),
				PW5_MYDEFINATIONREMARKS__ITEM(NUMFONTS,      "Number of device fonts"),
				// For high−color and full−color display resolutions NUMCOLORS often return -1
				// -1 is unreliable, so we can use this formular: NUMCOLORS =  1 << (number of color planes * color bits per pixel)
				PW5_MYDEFINATIONREMARKS__ITEM(NUMCOLORS,     "Number of device colors"),
				PW5_MYDEFINATIONREMARKS__ITEM(PDEVICESIZE,   "Size of device structure"),
				// ASPECTX/ASPECTY/ASPECTXY are related to the video dimensions,
				// relative width, height, and diagonal size of each pixel
				// ASPECTXY = square root of (squares(ASPECTX) + squares(ASPECTY))
				PW5_MYDEFINATIONREMARKS__ITEM(ASPECTX,       "Relative width of pixel"),
				PW5_MYDEFINATIONREMARKS__ITEM(ASPECTY,       "Relative height of pixel"),
				PW5_MYDEFINATIONREMARKS__ITEM(ASPECTXY,      "Relative diagonal of pixel"),
				// https://learn.microsoft.com/en-us/windows/win32/learnwin32/dpi-and-device-independent-pixels
					// dots per inch(DPI): The term dots derives from printing, where physical dots of ink are put onto paper
					// pixels per inch(PPI): For computer displays
						// 96 DPI: 72 points(dots) == 96 dots per inch(DPI).
						// 144 DPI: 72 points(dots) == 144 dots per inch(DPI) == 150% of 96 DPI
				PW5_MYDEFINATIONREMARKS__ITEM(LOGPIXELSX,    "(pixels)dots/points per logical inch X"),
				PW5_MYDEFINATIONREMARKS__ITEM(LOGPIXELSY,    "(pixels)dots/points per logical inch Y"),
				PW5_MYDEFINATIONREMARKS__ITEM(SIZEPALETTE,   "Number of palette entries"),
				// 256−color video adapters use color palettes, 
				// In that case, GetDeviceCaps with the NUMCOLORS index returns the number of colors reserved by Windows, which will be 20.
				// The remaining 236 colors can be set by a Windows program using the palette manager. 256 - 20 = 236
				PW5_MYDEFINATIONREMARKS__ITEM(NUMRESERVED,   "Reserved palette entries"),
				PW5_MYDEFINATIONREMARKS__ITEM(COLORRES,      "Actual color resolution")
			};
			set(dm, &s_eval);
		}
	};

	static DM info;
	return &info;
}


void PW5_DevCaps1::onCreate(CreateDesc& desc) {
	_dm = MyDevcapsDM::s_getRemarks();
	Base::onCreate(desc);
}

}

#endif // SGE_OS_WINDOWS