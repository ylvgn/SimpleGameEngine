#include "NativeUIWindow_Win32.h"
#include "Win32Util.h"
#include <sge_core/string/UtfUtil.h>

#if SGE_OS_WINDOWS

namespace sge {

void NativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	const wchar_t* clsName = L"NativeUIWindow";

	auto hInstance = ::GetModuleHandle(nullptr);
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

	auto rect = desc.rect;
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

	_hdc = GetDC(_hwnd);
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

LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
		}break;

		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd)) {
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
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
		// One flag that might be useful is bit 30, the "previous key state" flag, which is set to 1 for repeated key - down messages.

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
		case WM_SYSKEYDOWN: case WM_KEYDOWN:	{ ev.type = Type::Down; } break;
		case WM_SYSKEYUP:	case WM_KEYUP:		{ ev.type = Type::Up;	} break;
		case WM_SYSCHAR:	case WM_CHAR:		{ ev.type = Type::Char; } break;
	};

	switch (msg) {
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP: {

			#define SGE_E(K, SGE_T) case K: { /*SGE_LOG("{} = {}", #K, K);*/ ev.keyCode = KeyCode::SGE_T; } break;
			switch (wParam) {
				SGE_E(VK_CONTROL,		Ctrl)
				SGE_E(VK_SHIFT,			Shift)
				SGE_E(VK_MENU,			Alt)
				SGE_E(VK_LWIN,			LCmd)
				SGE_E(VK_RWIN,			RCmd)

				SGE_E(VK_RETURN,		Enter)
				SGE_E(VK_ESCAPE,		Escape)
				SGE_E(VK_TAB,			Tab)
				SGE_E(VK_CAPITAL,		CapsLock)
				SGE_E(VK_SPACE,			Space)
				SGE_E(VK_BACK,			Backspace)

				SGE_E(VK_OEM_1,			Semicolon)		// ;:
				SGE_E(VK_OEM_2,			Slash)			// /?
				SGE_E(VK_OEM_3,			BackQuote)		// `~
				SGE_E(VK_OEM_4,			LeftBracket)	// [{
				SGE_E(VK_OEM_5,			Backslash)		// \|
				SGE_E(VK_OEM_6,			RightBracket)	// ]}
				SGE_E(VK_OEM_7,			Quote)			// '~
				SGE_E(VK_OEM_PLUS,		Equals)			// =+
				SGE_E(VK_OEM_MINUS,		Hyphen)			// -_
				SGE_E(VK_OEM_COMMA,		Comma)			// ,<
				SGE_E(VK_OEM_PERIOD,	Period)			// .>

				SGE_E(VK_F1,	F1)
				SGE_E(VK_F2,	F2)
				SGE_E(VK_F3,	F3)
				SGE_E(VK_F4,	F4)
				SGE_E(VK_F5,	F5)
				SGE_E(VK_F6,	F6)
				SGE_E(VK_F7,	F7)
				SGE_E(VK_F8,	F8)
				SGE_E(VK_F9,	F9)
				SGE_E(VK_F10,	F10)
				SGE_E(VK_F11,	F11)
				SGE_E(VK_F12,	F12)

				SGE_E('0', Alpha0)	// 1!
				SGE_E('1', Alpha1)	// 2@
				SGE_E('2', Alpha2)	// 3#
				SGE_E('3', Alpha3)	// 4$
				SGE_E('4', Alpha4)	// 5%
				SGE_E('5', Alpha5)	// 6^
				SGE_E('6', Alpha6)	// 7&
				SGE_E('7', Alpha7)	// 8*
				SGE_E('8', Alpha8)	// 9(
				SGE_E('9', Alpha9)	// 10)

				SGE_E('A', A)
				SGE_E('B', B)
				SGE_E('C', C)
				SGE_E('D', D)
				SGE_E('E', E)
				SGE_E('F', F)
				SGE_E('G', G)
				SGE_E('H', H)
				SGE_E('I', I)
				SGE_E('J', J)
				SGE_E('K', K)
				SGE_E('L', L)
				SGE_E('M', M)
				SGE_E('N', N)
				SGE_E('O', O)
				SGE_E('P', P)
				SGE_E('Q', Q)
				SGE_E('R', R)
				SGE_E('S', S)
				SGE_E('T', T)
				SGE_E('U', U)
				SGE_E('V', V)
				SGE_E('W', W)
				SGE_E('X', X)
				SGE_E('Y', Y)
				SGE_E('Z', Z)

				SGE_E(VK_UP,	UpArrow)
				SGE_E(VK_DOWN,	DownArrow)
				SGE_E(VK_LEFT,	LeftArrow)
				SGE_E(VK_RIGHT,	RightArrow)

				SGE_E(VK_INSERT,	Insert)
				SGE_E(VK_DELETE,	Delete)
				SGE_E(VK_HOME,		Home)
				SGE_E(VK_END,		End)
				SGE_E(VK_PRIOR,		PageUp)
				SGE_E(VK_NEXT,		PageDown)

				SGE_E(VK_NUMPAD0,	Keypad0)
				SGE_E(VK_NUMPAD1,	Keypad1)
				SGE_E(VK_NUMPAD2,	Keypad2)
				SGE_E(VK_NUMPAD3,	Keypad3)
				SGE_E(VK_NUMPAD4,	Keypad4)
				SGE_E(VK_NUMPAD5,	Keypad5)
				SGE_E(VK_NUMPAD6,	Keypad6)
				SGE_E(VK_NUMPAD7,	Keypad7)
				SGE_E(VK_NUMPAD8,	Keypad8)
				SGE_E(VK_NUMPAD9,	Keypad9)

				SGE_E(VK_SNAPSHOT,	PrintScreen)
				SGE_E(VK_SCROLL,	ScrollLock)
				SGE_E(VK_PAUSE,		Pause)
				SGE_E(VK_NUMLOCK,	NumLock)
				SGE_E(VK_DIVIDE,	KeypadDivide)	// /
				SGE_E(VK_MULTIPLY,	KeypadMultiply)	// *
				SGE_E(VK_SUBTRACT,	KeypadMinus)	// -
				SGE_E(VK_ADD,		KeypadPlus)		// +
				SGE_E(VK_DECIMAL,	KeypadPeriod)	// .
			}
			#undef SGE_E
			break;
		}
		case WM_CHAR: { ev.charCode = static_cast<u32>(wParam); } break;
		default: return false;
	}

	// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-char
	if (ev.charCode > 32) {
		ev.charCodeStr = static_cast<char>(ev.charCode);
	}

	onUINativeKeyboardEvent(ev);
	return true;
}

LRESULT NativeUIWindow_Win32::_handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (_handleNativeUIMouseEvent(hwnd, msg, wParam, lParam)) return 0;
	if (_handleNativeUIKeyboardEvent(hwnd, msg, wParam, lParam)) return 0;
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

UIEventModifier NativeUIWindow_Win32::_getWin32Modifier() {
	auto o = UIEventModifier::None;
	if (::GetAsyncKeyState(VK_CONTROL)) {
		o |= UIEventModifier::Ctrl;
		if (::GetAsyncKeyState(VK_LCONTROL)) o |= UIEventModifier::LCtrl;
		if (::GetAsyncKeyState(VK_RCONTROL)) o |= UIEventModifier::RCtrl;
	}
	if (::GetAsyncKeyState(VK_SHIFT)) {
		o |= UIEventModifier::Shift;
		if (::GetAsyncKeyState(VK_LSHIFT)) o |= UIEventModifier::LShift;
		if (::GetAsyncKeyState(VK_RSHIFT)) o |= UIEventModifier::RShift;
	}
	if (::GetAsyncKeyState(VK_MENU)) {
		o |= UIEventModifier::Alt;
		if (::GetAsyncKeyState(VK_LMENU)) o |= UIEventModifier::LAlt;
		if (::GetAsyncKeyState(VK_RMENU)) o |= UIEventModifier::RAlt;
	}
	if (::GetAsyncKeyState(VK_LWIN) || ::GetAsyncKeyState(VK_RWIN)) {
		o |= UIEventModifier::Cmd;
		if (::GetAsyncKeyState(VK_LWIN)) o |= UIEventModifier::LCmd;
		if (::GetAsyncKeyState(VK_RWIN)) o |= UIEventModifier::RCmd;
	}
	return o;
}

}

#endif
