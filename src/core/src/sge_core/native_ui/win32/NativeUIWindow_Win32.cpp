#include "NativeUIWindow_Win32.h"
#include "Win32Util.h"
#include <sge_core/string/UtfUtil.h>

#if SGE_OS_WINDOWS

namespace sge {

void NativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	const wchar_t* clsName = L"NativeUIWindow";

	auto hInstance		= ::GetModuleHandle(nullptr);
	WNDCLASSEX wc       = {}; // ZeroMemory
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW; // | CS_DROPSHADOW;
	wc.lpfnWndProc		= &s_wndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(hInstance, IDC_ARROW);
	wc.hbrBackground	= nullptr; //(HBRUSH)(COLOR_WINDOW+1); // Modify the window to have no background, leaves the background color untouched
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= clsName;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	if (!desc.closeButton) {
		wc.style |= CS_NOCLOSE;
	}
	if (desc.ownDC) {
		wc.style |= CS_OWNDC;
	}

	DWORD dwStyle = 0;
	DWORD dwExStyle = WS_EX_ACCEPTFILES;

	if (desc.alwaysOnTop) {
		dwExStyle |= WS_EX_TOPMOST;
	}

	switch (desc.type) {
		case CreateDesc::Type::ToolWindow:
		case CreateDesc::Type::NormalWindow: {
			dwStyle   |= WS_OVERLAPPED | WS_SYSMENU;

			if (desc.closeButton) dwStyle |= WS_SYSMENU;
			if (desc.resizable  ) dwStyle |= WS_THICKFRAME;
			if (desc.titleBar   ) dwStyle |= WS_CAPTION;
			if (desc.minButton  ) dwStyle |= WS_MINIMIZEBOX;
			if (desc.maxButton  ) dwStyle |= WS_MAXIMIZEBOX;
			if (desc.vScrollBar ) dwStyle |= WS_VSCROLL;
			if (desc.hScrollBar ) dwStyle |= WS_HSCROLL;
		}break;

		case CreateDesc::Type::PopupWindow: {
			dwStyle   |= WS_POPUP | WS_BORDER;
		}break;

		default: {
			SGE_ASSERT(false);
		}break;
	}

	if (desc.type == CreateDesc::Type::ToolWindow) {
		dwExStyle |= WS_EX_TOOLWINDOW;
	}

	WNDCLASSEX tmpWc;
	bool registered = (0 != ::GetClassInfoEx(hInstance, clsName, &tmpWc));
	if (!registered) {
		if (!::RegisterClassEx(&wc)) {
			throw SGE_ERROR("error RegisterClassEx");
		}
	}

	auto& rect = desc.rect;
	if (desc.centerToScreen) {
		int screenWidth	 = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		auto screenSize = Vec2f(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
		rect.pos = (screenSize - rect.size) / 2;
	}

	_hwnd = ::CreateWindowEx(dwExStyle, clsName, clsName, dwStyle,
								static_cast<int>(desc.rect.x),
								static_cast<int>(desc.rect.y),
								static_cast<int>(desc.rect.w),
								static_cast<int>(desc.rect.h),
								nullptr, nullptr, hInstance, this);
	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

	if (desc.vScrollBar) {
		ScrollInfo::CreateDesc vScrollInfo(ScrollInfo::CreateDesc::Axis::Vertical);
		_vScrollInfo.reset(static_cast<ScrollInfo*>(createScrollBar()));
		_vScrollInfo->create(vScrollInfo);
	}
	if (desc.hScrollBar) {
		ScrollInfo::CreateDesc hScrollInfo(ScrollInfo::CreateDesc::Axis::Horizontal);
		_hScrollInfo.reset(static_cast<ScrollInfo*>(createScrollBar()));
		_hScrollInfo->create(hScrollInfo);
	}

	ShowWindow(_hwnd, SW_SHOW);
}

void NativeUIWindow_Win32::onSetWindowTitle(StrView title) {
	if (!_hwnd) return;
	TempStringW tmp = UtfUtil::toStringW(title);
	::SetWindowText(_hwnd, tmp.c_str());
}

void NativeUIWindow_Win32::onSetCursor(UIMouseCursor type) {
	if (!_hwnd) return;

	using Cursor	= UIMouseCursor;
	LPTSTR cursor	= IDC_ARROW;
	switch (type)
	{
	case Cursor::Arrow:		cursor = IDC_ARROW;		break;
	case Cursor::IBeam:		cursor = IDC_IBEAM;		break;
	case Cursor::SizeAll:	cursor = IDC_SIZEALL;	break;
	case Cursor::SizeWE:	cursor = IDC_SIZEWE;	break;
	case Cursor::SizeNS:	cursor = IDC_SIZENS;	break;
	case Cursor::SizeNESW:	cursor = IDC_SIZENESW;	break;
	case Cursor::SizeNWSE:	cursor = IDC_SIZENWSE;	break;
	case Cursor::Hand:		cursor = IDC_HAND;		break;
	case Cursor::No:		cursor = IDC_NO;		break;
	case Cursor::None:		::SetCursor(NULL);		return;
	}
	::SetCursor(LoadCursor(0, cursor));
}

void NativeUIWindow_Win32::onDrawNeeded() {
	::InvalidateRect(_hwnd, nullptr, false);
}

NativeUIWindow_Win32::Base::ScrollInfo* NativeUIWindow_Win32::onCreateScrollBar() {
	return new NativeUIScrollInfo_Win32();
}

LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
		}break;

		case WM_SHOWWINDOW: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onShow();
			}
		}break;

		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd)) {
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
				thisObj->_hwnd = nullptr;
				sge_delete(thisObj);
			}
		}break;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onDraw();
			}
			EndPaint(hwnd, &ps);
			return 0;
		}break;

		case WM_CLOSE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onCloseButton();
				return 0;
			}
		}break;

		case WM_SIZE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				RECT clientRect;
				::GetClientRect(hwnd, &clientRect);
				Rect2f newClientRect = Win32Util::toRect2f(clientRect);

				thisObj->onClientRectChanged(newClientRect);
				return 0;
			}
 		}break;

		case WM_ACTIVATE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				u16 a = LOWORD(wParam);
				switch (a) {
					case WA_ACTIVE:		thisObj->onActive(true);  break;
					case WA_CLICKACTIVE:thisObj->onActive(true);  break;
					case WA_INACTIVE:	thisObj->onActive(false); break;
				}
			}
		}break;

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				return thisObj->_handleNativeEvent(hwnd, msg, wParam, lParam);
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

bool NativeUIWindow_Win32::_handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	UIMouseEvent ev;

	ev.modifier = _getWin32Modifier();

	using Button = UIMouseEventButton;
	using Type   = UIMouseEventType;

	POINT curPos;
	::GetCursorPos(&curPos);
	::ScreenToClient(hwnd, &curPos);

	Win32Util::convert(ev.pos, curPos);

	// the 'auto button' no need, why???
	auto button = Button::None;
	switch (HIWORD(wParam)) {
		case XBUTTON1: button = Button::Button4; break;
		case XBUTTON2: button = Button::Button5; break;
	}

	switch (msg) {
		case WM_LBUTTONUP:		{ ev.type = Type::Up;	ev.button = Button::Left;	} break;
		case WM_MBUTTONUP:		{ ev.type = Type::Up;	ev.button = Button::Middle;	} break;
		case WM_RBUTTONUP:		{ ev.type = Type::Up;	ev.button = Button::Right;	} break;

		case WM_LBUTTONDOWN:	{ ev.type = Type::Down;	ev.button = Button::Left;	} break;
		case WM_MBUTTONDOWN:	{ ev.type = Type::Down;	ev.button = Button::Middle;	} break;
		case WM_RBUTTONDOWN:	{ ev.type = Type::Down;	ev.button = Button::Right;	} break;

		case WM_MOUSEMOVE:		{ ev.type = Type::Move;	} break;

	#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
		// vertical  scroll wheel 
		case WM_MOUSEWHEEL:		{ ev.type = Type::Scroll;	ev.scroll.set(0,GET_WHEEL_DELTA_WPARAM(wParam)); } break;
	#endif

	#if (_WIN32_WINNT >= 0x0600)
		// horizontal scroll wheel 
		case WM_MOUSEHWHEEL:	{ ev.type = Type::Scroll;	ev.scroll.set(GET_WHEEL_DELTA_WPARAM(wParam),0); } break;
	#endif

		default:
			return false;
	}

	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setcapture
	switch (ev.type) {
		case Type::Down:	::SetCapture(hwnd); break;
		case Type::Up:		::ReleaseCapture(); break;
	}

	onUINativeMouseEvent(ev);
	return true;
}

bool NativeUIWindow_Win32::_handleNativeUIKeyboardEvent(HWND hwnd,
														UINT msg,
														WPARAM wParam,
														LPARAM lParam)
{
	// https://learn.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input
	// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

	// If you intercept the WM_SYSKEYDOWN message, call DefWindowProc afterward.
	// Otherwise, you will block the operating system from handling the command.

	// WM_CHAR messages are generated from WM_KEYDOWN messages
		// while WM_SYSCHAR messages are generated from WM_SYSKEYDOWN messages.
		// As with WM_SYSKEYDOWN, you should generally pass this message directly to DefWindowProc.
		// In particular, do not treat WM_SYSCHAR as text that the user has typed.

	// the wParam parameter contains the virtual-key code of the key
	// The lParam parameter contains some miscellaneous information packed into 32 bits.
		// You typically do not need the information in lParam
		// One flag that might be useful is bit 30, the "previous key state" flag, which is set to 1 for repeated key-down messages.

	UIKeyboardEvent ev;

	ev.modifier = _getWin32Modifier();

	using KeyCode	= UIKeyboardEvent::KeyCode;
	using Type		= UIKeyboardEvent::Type;

#if 0
	switch (msg) {
		case WM_SYSKEYDOWN: SGE_LOG("WM_SYSKEYDOWN: 0x{:x}",	wParam); break;
		case WM_SYSCHAR:	SGE_LOG("WM_SYSCHAR: {:c}",			wParam); break;
		case WM_SYSKEYUP:	SGE_LOG("WM_SYSKEYUP: 0x{:x}",		wParam); break;
		case WM_KEYDOWN:	SGE_LOG("WM_KEYDOWN: 0x{:x}",		wParam); break;
		case WM_KEYUP:		SGE_LOG("WM_KEYUP: 0x{:x}",			wParam); break;
		case WM_CHAR:		SGE_LOG("WM_CHAR: {:c}",			wParam); break;
	};
#endif

	switch (msg) {
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:{
			ev.type = Type::Down;
		} break;
		case WM_SYSKEYUP:
		case WM_KEYUP:{
			ev.type = Type::Up;
		} break;
		case WM_SYSCHAR:
		case WM_CHAR:{
			ev.type = Type::Char;
		} break;
	};

	switch (msg) {
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP: {
			
//			SGE_LOG("GetKeyState({})= 0x{:x}", wParam, ::GetKeyState(static_cast<int>(wParam)));

			#define CASE_1(K, SGE_T) case VK_##K: { \
					auto lIsDown = ::GetKeyState(VK_L##K) & 0x8000; \
					auto rIsDown = ::GetKeyState(VK_R##K) & 0x8000; \
					if (ev.type == Type::Down) { \
						if (lIsDown) { auto k = KeyCode::L##SGE_T; _keyCodesMap[k] = Type::Down; ev.keyCode = k; } \
						if (rIsDown) { _keyCodesMap[KeyCode::R##SGE_T] = Type::Down; } \
						if (ev.keyCode == KeyCode::None) ev.keyCode = KeyCode::R##SGE_T; \
					} else { \
						if (isKeyDown(KeyCode::L##SGE_T) && !lIsDown) { auto k = KeyCode::L##SGE_T; _keyCodesMap[k] = Type::Up; ev.keyCode = k; } \
						if (isKeyDown(KeyCode::R##SGE_T) && !rIsDown) { _keyCodesMap[KeyCode::R##SGE_T] = Type::Up; } \
						if (ev.keyCode == KeyCode::None) ev.keyCode = KeyCode::R##SGE_T; \
					} \
					_keyCodesMap[ev.keyCode] = ev.type; \
				} break; \
			// -----
			#define CASE_2(K, SGE_T) case K: { /*SGE_LOG("{} = {}", #K, K);*/ ev.keyCode = KeyCode::SGE_T; } break;
			switch (wParam) {
				CASE_1(CONTROL, Ctrl)
				CASE_1(SHIFT,	Shift)
				CASE_1(MENU,	Alt)

				CASE_2(VK_LWIN,	LCmd)
				CASE_2(VK_RWIN,	RCmd)

				CASE_2(VK_RETURN,		Enter)
				CASE_2(VK_ESCAPE,		Escape)
				CASE_2(VK_TAB,			Tab)
				CASE_2(VK_CAPITAL,		CapsLock)
				CASE_2(VK_SPACE,		Space)
				CASE_2(VK_BACK,			Backspace)

				CASE_2(VK_OEM_1,		Semicolon)		// ;:
				CASE_2(VK_OEM_2,		Slash)			// /?
				CASE_2(VK_OEM_3,		BackQuote)		// `~
				CASE_2(VK_OEM_4,		LeftBracket)	// [{
				CASE_2(VK_OEM_5,		Backslash)		// \|
				CASE_2(VK_OEM_6,		RightBracket)	// ]}
				CASE_2(VK_OEM_7,		Quote)			// '~
				CASE_2(VK_OEM_PLUS,		Equals)			// =+
				CASE_2(VK_OEM_MINUS,	Hyphen)			// -_
				CASE_2(VK_OEM_COMMA,	Comma)			// ,<
				CASE_2(VK_OEM_PERIOD,	Period)			// .>

				CASE_2(VK_F1,	F1)
				CASE_2(VK_F2,	F2)
				CASE_2(VK_F3,	F3)
				CASE_2(VK_F4,	F4)
				CASE_2(VK_F5,	F5)
				CASE_2(VK_F6,	F6)
				CASE_2(VK_F7,	F7)
				CASE_2(VK_F8,	F8)
				CASE_2(VK_F9,	F9)
				CASE_2(VK_F10,	F10)
				CASE_2(VK_F11,	F11)
				CASE_2(VK_F12,	F12)

				CASE_2('0', Alpha0)	// 1!
				CASE_2('1', Alpha1)	// 2@
				CASE_2('2', Alpha2)	// 3#
				CASE_2('3', Alpha3)	// 4$
				CASE_2('4', Alpha4)	// 5%
				CASE_2('5', Alpha5)	// 6^
				CASE_2('6', Alpha6)	// 7&
				CASE_2('7', Alpha7)	// 8*
				CASE_2('8', Alpha8)	// 9(
				CASE_2('9', Alpha9)	// 10)

				CASE_2('A', A)
				CASE_2('B', B)
				CASE_2('C', C)
				CASE_2('D', D)
				CASE_2('E', E)
				CASE_2('F', F)
				CASE_2('G', G)
				CASE_2('H', H)
				CASE_2('I', I)
				CASE_2('J', J)
				CASE_2('K', K)
				CASE_2('L', L)
				CASE_2('M', M)
				CASE_2('N', N)
				CASE_2('O', O)
				CASE_2('P', P)
				CASE_2('Q', Q)
				CASE_2('R', R)
				CASE_2('S', S)
				CASE_2('T', T)
				CASE_2('U', U)
				CASE_2('V', V)
				CASE_2('W', W)
				CASE_2('X', X)
				CASE_2('Y', Y)
				CASE_2('Z', Z)

				CASE_2(VK_UP,		UpArrow)
				CASE_2(VK_DOWN,		DownArrow)
				CASE_2(VK_LEFT,		LeftArrow)
				CASE_2(VK_RIGHT,	RightArrow)

				CASE_2(VK_INSERT,	Insert)
				CASE_2(VK_DELETE,	Delete)
				CASE_2(VK_HOME,		Home)
				CASE_2(VK_END,		End)
				CASE_2(VK_PRIOR,	PageUp)
				CASE_2(VK_NEXT,		PageDown)

				CASE_2(VK_NUMPAD0,	Keypad0)
				CASE_2(VK_NUMPAD1,	Keypad1)
				CASE_2(VK_NUMPAD2,	Keypad2)
				CASE_2(VK_NUMPAD3,	Keypad3)
				CASE_2(VK_NUMPAD4,	Keypad4)
				CASE_2(VK_NUMPAD5,	Keypad5)
				CASE_2(VK_NUMPAD6,	Keypad6)
				CASE_2(VK_NUMPAD7,	Keypad7)
				CASE_2(VK_NUMPAD8,	Keypad8)
				CASE_2(VK_NUMPAD9,	Keypad9)

				CASE_2(VK_SNAPSHOT,	PrintScreen)
				CASE_2(VK_SCROLL,	ScrollLock)
				CASE_2(VK_PAUSE,	Pause)
				CASE_2(VK_NUMLOCK,	NumLock)
				CASE_2(VK_DIVIDE,	KeypadDivide)	// /
				CASE_2(VK_MULTIPLY,	KeypadMultiply)	// *
				CASE_2(VK_SUBTRACT,	KeypadMinus)	// -
				CASE_2(VK_ADD,		KeypadPlus)		// +
				CASE_2(VK_DECIMAL,	KeypadPeriod)	// .
			}
			#undef CASE_1
			#undef CASE_2
			break;
		}
		case WM_CHAR: { ev.charCode = static_cast<u32>(wParam); } break;
		default: return false;
	}

	// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-char
	if (ev.charCode > 32) {
		ev.charCodeStr = static_cast<char>(ev.charCode);
	}

	// reset all keycode
	for (auto& kv : _keyCodesMap) {
		auto k = kv.first;

		if (k == KeyCode::LCtrl
			|| k == KeyCode::RCtrl
			//|| k == KeyCode::LShift
			//|| k == KeyCode::RShift
			|| k == KeyCode::LAlt
			|| k == KeyCode::RAlt
		) {
			continue;
		}

		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate
		auto state = ::GetKeyState(Win32Util::toVKKey(k));
		if (state & 0x8000) {
			_keyCodesMap[k] = Type::Down;
		} else if (state == 1) {
			_keyCodesMap[k] = Type::Up;
		} else {
			_keyCodesMap[k] = Type::None;
		}
	}

	onUINativeKeyboardEvent(ev);
	return true;
}

bool NativeUIWindow_Win32::_handleNativeUIScrollBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	using Mode = UIScrollBarEvent::Mode;

	UIScrollBarEvent ev;

	switch(msg) {
		case WM_HSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				int request = LOWORD(wParam);
				auto& si = thisObj->_hScrollInfo;
				int oldPos;
				si->getPos(hwnd, oldPos);
				switch (request) {
				case SB_LINELEFT:		ev.mode = Mode::Step;   si->scrollTo(oldPos - si->step());	break;
					case SB_LINERIGHT:	ev.mode = Mode::Step;   si->scrollTo(oldPos + si->step());	break;
					case SB_PAGELEFT:	ev.mode = Mode::Page;   si->scrollTo(oldPos - si->page());	break;
					case SB_PAGERIGHT:	ev.mode = Mode::Page;   si->scrollTo(oldPos + si->page());	break;
					case SB_THUMBTRACK:	ev.mode = Mode::Scroll; si->scrollTo(HIWORD(wParam));		break;
				}
				ev.pos.x = si->pos();
				ev.deltaPos.x = ev.pos.x - oldPos;
			}
		} break;

		case WM_VSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				int request = LOWORD(wParam);
				auto& si = thisObj->_vScrollInfo;
				int oldPos;
				si->getPos(hwnd, oldPos);
				switch (request) {
					case SB_LINEUP:		ev.mode = Mode::Step;   si->scrollTo(oldPos - si->step());	break;
					case SB_LINEDOWN:	ev.mode = Mode::Step;   si->scrollTo(oldPos + si->step());	break;
					case SB_PAGEUP:		ev.mode = Mode::Page;   si->scrollTo(oldPos - si->page());	break;
					case SB_PAGEDOWN:	ev.mode = Mode::Page;   si->scrollTo(oldPos + si->page());	break;
					case SB_THUMBTRACK:	ev.mode = Mode::Scroll; si->scrollTo(HIWORD(wParam));		break;
				}
				ev.pos.y = si->pos();
				ev.deltaPos.y = ev.pos.y - oldPos;
			}
		} break;

		default:
			return false;
	}

	if (ev.deltaPos.x || ev.deltaPos.y) {
		::ScrollWindow(_hwnd, -ev.deltaPos.x, -ev.deltaPos.y, nullptr, nullptr);
		onUINativeScrollBarEvent(ev);
	}

	return true;
}

void NativeUIWindow_Win32::onScrollWindow(const Vec2i& delta) {
	int oldPos;
	if (delta.x) {
		_hScrollInfo->getPos(_hwnd, oldPos);
		int newPos = oldPos + delta.x;
		WPARAM wParam = MAKELONG(SB_THUMBTRACK, newPos);
		_handleNativeUIScrollBarEvent(_hwnd, WM_HSCROLL, wParam, 0);
	}
	if (delta.y) {
		_vScrollInfo->getPos(_hwnd, oldPos);
		int newPos = oldPos + delta.y;
		WPARAM wParam = MAKELONG(SB_THUMBTRACK, newPos);
		_handleNativeUIScrollBarEvent(_hwnd, WM_VSCROLL, wParam, 0);
	}
}

LRESULT NativeUIWindow_Win32::_handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (_handleNativeUIMouseEvent(hwnd, msg, wParam, lParam)) return 0;
	if (_handleNativeUIKeyboardEvent(hwnd, msg, wParam, lParam)) return 0;
	if (_handleNativeUIScrollBarEvent(hwnd, msg, wParam, lParam)) return 0;
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

UIEventModifier NativeUIWindow_Win32::_getWin32Modifier() {
	auto o = UIEventModifier::None;
	if (::GetAsyncKeyState(VK_CONTROL)) o |= UIEventModifier::Ctrl;
	if (::GetAsyncKeyState(VK_SHIFT)) o |= UIEventModifier::Shift;
	if (::GetAsyncKeyState(VK_MENU)) o |= UIEventModifier::Alt;
	if (::GetAsyncKeyState(VK_LWIN) || ::GetAsyncKeyState(VK_RWIN)) {
		o |= UIEventModifier::Cmd;
	}
	return o;
}

}

#endif
