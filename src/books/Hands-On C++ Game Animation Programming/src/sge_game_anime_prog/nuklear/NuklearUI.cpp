#include "NuklearUI.h"

NK_API struct nk_context* nk_win32_init();
NK_API int	nk_win32_handle_event(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, float scaleFactor);
NK_API void nk_win32_render(int width, int height, int display_width, int display_height);
NK_API void nk_win32_shutdown(void);

NK_API void nk_win32_device_destroy(void);
NK_API void nk_win32_device_create(void);
NK_API void nk_win32_font_stash_begin(struct nk_font_atlas** atlas);
NK_API void nk_win32_font_stash_end(void);

NK_API void nk_demo();

namespace sge {
namespace NuklearUI {

//static nk_context* NuklearUI::s_ctx = nullptr;

void createContext() {
	SGE_ASSERT(s_ctx == nullptr);
	s_ctx = nk_win32_init();
}

void destroyContext() {
	if (s_ctx) {
		nk_win32_shutdown();
		s_ctx = nullptr;
	}
}

void render(int width,
	int height,
	int displayWidth,
	int displayHeight)
{
	nk_win32_render(width, height, displayWidth, displayHeight);
}

void handleWindowEvent(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam,
	float scaleFactor)
{
	nk_win32_handle_event(hwnd, msg, wparam, lparam, scaleFactor);
}


void demo()	{
	nk_demo();
}

}
}