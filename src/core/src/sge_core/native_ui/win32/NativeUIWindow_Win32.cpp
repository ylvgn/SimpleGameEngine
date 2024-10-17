#include "NativeUIWindow_Win32.h"
#include "Win32Util.h"
#include <sge_core/string/UtfUtil.h>

#if SGE_OS_WINDOWS

namespace sge {

void NativeUIWindow_Win32::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	const wchar_t* clsName = L"NativeUIWindow";

	auto hInstance		= GetModuleHandle(nullptr);
	WNDCLASSEX wc       = {}; // ZeroMemory
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW; // | CS_DROPSHADOW;
	wc.lpfnWndProc		= &s_wndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(hInstance, IDC_ARROW);
	wc.hbrBackground	= nullptr; // (HBRUSH)(COLOR_WINDOW+1); // Modify the window to have no background, leaves the background color untouched
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
	bool registered = (0 != GetClassInfoEx(hInstance, clsName, &tmpWc));
	if (!registered) {
		if (!RegisterClassEx(&wc)) {
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

	_hwnd = CreateWindowEx(dwExStyle, clsName, clsName, dwStyle,
							 int(desc.rect.x),
							 int(desc.rect.y),
							 int(desc.rect.w),
							 int(desc.rect.h),
							 nullptr, nullptr, hInstance, this);

	if (!_hwnd) {
		throw SGE_ERROR("cannot create native window");
	}

	{
		using ScrollInfo_CreateDesc = NativeUIScrollInfo_CreateDesc;
		using ScrollInfo_CreateDesc_Axis = ScrollInfo_CreateDesc::Axis;

		if (desc.vScrollBar) {
			ScrollInfo_CreateDesc scrollInfoDesc(ScrollInfo_CreateDesc_Axis::Vertical);
			scrollInfoDesc.page = static_cast<u32>(desc.rect.h);
			auto* p = createScrollBar(scrollInfoDesc).detach();
			_vScrollInfo.reset(static_cast<NativeUIScrollInfo_Win32*>(p));
		}

		if (desc.hScrollBar) {
			ScrollInfo_CreateDesc scrollInfoDesc(ScrollInfo_CreateDesc_Axis::Horizontal);
			scrollInfoDesc.page = static_cast<u32>(desc.rect.w);
			auto* p = createScrollBar(scrollInfoDesc).detach();
			_hScrollInfo.reset(static_cast<NativeUIScrollInfo_Win32*>(p));
		}
	}

	_pressedKeyCodes.resize(kKeyCodeCount);

	// TODO call 'ShowWindow' when created render context
	ShowWindow(_hwnd, SW_SHOW);
	//UpdateWindow(_hwnd); // !!<--- do not call UpdateWindow, default OS will call it.
}

void NativeUIWindow_Win32::onSetWindowTitle(StrView title) {
	if (!_hwnd) return;
	TempStringW tmp = UtfUtil::toStringW(title);
	SetWindowText(_hwnd, tmp.c_str());
}

void NativeUIWindow_Win32::onSetWindowPos(const Vec2f& pos) {
	Base::onSetWindowPos(pos);
	SetWindowPos(_hwnd, HWND_TOP, int(pos.x), int(pos.y), 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOSIZE);
}

void NativeUIWindow_Win32::onSetWindowSize(const Vec2f& size) {
	Base::onSetWindowPos(size);
	SetWindowPos(_hwnd, HWND_TOP, 0, 0, int(size.x), int(size.y), SWP_ASYNCWINDOWPOS | SWP_NOMOVE);
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
		case Cursor::None:		SetCursor(NULL);		return;
	}
	SetCursor(LoadCursor(0, cursor));
}

void NativeUIWindow_Win32::onDrawNeeded() {
	InvalidateRect(_hwnd, nullptr, false);
}

void NativeUIWindow_Win32::onScrollWindow(const Vec2i& delta) {
	int oldPos;
	if (delta.x) {
		_hScrollInfo->getPos(oldPos);
		int newPos = Math::max(0, oldPos + delta.x); // !!<--- must be positive
		_handleNativeUIScrollBarEvent(_hwnd, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, newPos), 0);
	}
	if (delta.y) {
		_vScrollInfo->getPos(oldPos);
		int newPos = Math::max(0, oldPos + delta.y); // !!<--- must be positive
		_handleNativeUIScrollBarEvent(_hwnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, newPos), 0);
	}
}

UPtr<NativeUIScrollInfo_Base> NativeUIWindow_Win32::onCreateScrollBar(NativeUIScrollInfo_Base::CreateDesc& desc) {
	return UPtr_make<NativeUIScrollInfo_Win32>(this, desc);
}

LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisObj));
		}break;

		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd)) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
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
				GetClientRect(hwnd, &clientRect);
				Rect2f newClientRect = Win32Util::toRect2f(clientRect);
				if (newClientRect != thisObj->_clientRect) {
					thisObj->onClientRectChanged(newClientRect);
				}
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
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool NativeUIWindow_Win32::_handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	UIMouseEvent ev;

	ev.modifier = _getWin32Modifier();

	using Button = UIMouseEventButton;
	using Type   = UIMouseEventType;

	POINT curPos;
	GetCursorPos(&curPos);
	ScreenToClient(hwnd, &curPos);

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
		case Type::Down:	SetCapture(hwnd); break;
		case Type::Up:		ReleaseCapture(); break;
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
	using Modifier	= UIEventModifier;
	using Type		= UIKeyboardEvent::Type;

#if 0
	switch (msg) {
		case WM_SYSKEYDOWN: SGE_LOG("WM_SYSKEYDOWN: wParam=0x{:x}, lParam=0x{:x}, GetKeyState(0x{:x})={}", wParam, lParam, wParam, GetKeyState(static_cast<int>(wParam))); break;
		case WM_SYSCHAR:	SGE_LOG("WM_SYSCHAR: wParam={:c}, lParam=0x{:x}, GetKeyState(0x{:x})={}", wParam, lParam, wParam, GetKeyState(static_cast<int>(wParam))); break;
		case WM_SYSKEYUP:	SGE_LOG("WM_SYSKEYUP: wParam=0x{:x}, lParam=0x{:x}, GetKeyState(0x{:x})={}", wParam, lParam, wParam, GetKeyState(static_cast<int>(wParam))); break;
		case WM_KEYDOWN:	SGE_LOG("WM_KEYDOWN: wParam=0x{:x}, lParam=0x{:x}, GetKeyState(0x{:x})={}", wParam, lParam, wParam, GetKeyState(static_cast<int>(wParam))); break;
		case WM_KEYUP:		SGE_LOG("WM_KEYUP: wParam=0x{:x}, lParam=0x{:x}, GetKeyState(0x{:x})={}", wParam, lParam, wParam, GetKeyState(static_cast<int>(wParam))); break;
		case WM_CHAR:		SGE_LOG("WM_CHAR: wParam={:c}, lParam=0x{:x}, GetKeyState(0x{:x})={}", wParam, lParam, wParam, GetKeyState(static_cast<int>(wParam))); break;
	};
#endif

	switch (msg) {
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: { ev.type = Type::Down; } break;

		case WM_SYSKEYUP:
		case WM_KEYUP: { ev.type = Type::Up; } break;

		case WM_CHAR: { ev.charCode = static_cast<u32>(wParam); } break;
	};

	// Ctrl, Shift, Alt
	switch (msg) {
		case WM_KEYDOWN: {
			switch (wParam) {
				case VK_CONTROL: {
					if (_isKeyDown(VK_LCONTROL)) {
						_pressedKeyCodes[VK_LCONTROL] = Type::Down;
					}
					if (_isKeyDown(VK_RCONTROL)) {
						_pressedKeyCodes[VK_RCONTROL] = Type::Down;
					}
				} break;
				case VK_SHIFT: {
					if (_isKeyDown(VK_LSHIFT)) {
						_pressedKeyCodes[VK_LSHIFT] = Type::Down;
					}
					if (_isKeyDown(VK_RSHIFT)) {
						_pressedKeyCodes[VK_RSHIFT] = Type::Down;
					}
				} break;
				case VK_MENU: {
					if (_isKeyDown(VK_LMENU)) {
						_pressedKeyCodes[VK_LMENU] = Type::Down;
					}
					if (_isKeyDown(VK_RMENU)) {
						_pressedKeyCodes[VK_RMENU] = Type::Down;
					}
				} break;
			}
		} break;
		case WM_KEYUP: {
			switch (wParam) {
				case VK_CONTROL: {
					_pressedKeyCodes[VK_LCONTROL] = _isKeyDown(VK_LCONTROL) ? Type::Down : ev.type;
					_pressedKeyCodes[VK_RCONTROL] = _isKeyDown(VK_RCONTROL) ? Type::Down : ev.type;
				} break;
				case VK_SHIFT: {
					_pressedKeyCodes[VK_LSHIFT] = _isKeyDown(VK_LSHIFT) ? Type::Down : ev.type;
					_pressedKeyCodes[VK_RSHIFT] = _isKeyDown(VK_RSHIFT) ? Type::Down : ev.type;
				} break;
				case VK_MENU: {
					_pressedKeyCodes[VK_LMENU] = _isKeyDown(VK_LMENU) ? Type::Down : ev.type;
					_pressedKeyCodes[VK_RMENU] = _isKeyDown(VK_RMENU) ? Type::Down : ev.type;
				} break;
			}
		} break;
	}

	// Alt
	switch (msg) {
		case WM_SYSKEYDOWN: {
			switch (wParam) {
				case VK_MENU: {
					if (_isKeyDown(VK_LMENU)) {
						_pressedKeyCodes[VK_LMENU] = Type::Down;
					}
					if (_isKeyDown(VK_RMENU)) {
						_pressedKeyCodes[VK_RMENU] = Type::Down;
					}
				} break;
			}
		} break;
		case WM_SYSKEYUP: {
			switch (wParam) {
				case VK_MENU: {
					_pressedKeyCodes[VK_LMENU] = _isKeyDown(VK_LMENU) ? Type::Down : ev.type;
					_pressedKeyCodes[VK_RMENU] = _isKeyDown(VK_RMENU) ? Type::Down : ev.type;
				} break;
			}
		} break;
	}

	// Others
	switch (msg) {
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP: {
			_pressedKeyCodes[wParam] = ev.type;

			#define CASE_E(K, SGE_T) case K: { /*SGE_LOG("{} = {}", #K, K);*/ ev.keyCode = KeyCode::SGE_T; } break;
			switch (wParam) {
				CASE_E(VK_CONTROL,		Ctrl)
				CASE_E(VK_SHIFT,		Shift)
				CASE_E(VK_MENU,			Alt)
				CASE_E(VK_LWIN,			LCmd)
				CASE_E(VK_RWIN,			RCmd)
				CASE_E(VK_RETURN,		Enter)
				CASE_E(VK_ESCAPE,		Escape)
				CASE_E(VK_TAB,			Tab)
				CASE_E(VK_CAPITAL,		CapsLock)
				CASE_E(VK_SPACE,		Space)
				CASE_E(VK_BACK,			Backspace)
				CASE_E(VK_OEM_1,		Semicolon)		// ;:
				CASE_E(VK_OEM_2,		Slash)			// /?
				CASE_E(VK_OEM_3,		BackQuote)		// `~
				CASE_E(VK_OEM_4,		LeftBracket)	// [{
				CASE_E(VK_OEM_5,		Backslash)		// \|
				CASE_E(VK_OEM_6,		RightBracket)	// ]}
				CASE_E(VK_OEM_7,		Quote)			// '~
				CASE_E(VK_OEM_PLUS,		Equals)			// =+
				CASE_E(VK_OEM_MINUS,	Hyphen)			// -_
				CASE_E(VK_OEM_COMMA,	Comma)			// ,<
				CASE_E(VK_OEM_PERIOD,	Period)			// .>
				CASE_E(VK_F1,			F1)
				CASE_E(VK_F2,			F2)
				CASE_E(VK_F3,			F3)
				CASE_E(VK_F4,			F4)
				CASE_E(VK_F5,			F5)
				CASE_E(VK_F6,			F6)
				CASE_E(VK_F7,			F7)
				CASE_E(VK_F8,			F8)
				CASE_E(VK_F9,			F9)
				CASE_E(VK_F10,			F10)
				CASE_E(VK_F11,			F11)
				CASE_E(VK_F12,			F12)
				CASE_E('0',				Alpha0)			// 1!
				CASE_E('1',				Alpha1)			// 2@
				CASE_E('2',				Alpha2)			// 3#
				CASE_E('3',				Alpha3)			// 4$
				CASE_E('4',				Alpha4)			// 5%
				CASE_E('5',				Alpha5)			// 6^
				CASE_E('6',				Alpha6)			// 7&
				CASE_E('7',				Alpha7)			// 8*
				CASE_E('8',				Alpha8)			// 9(
				CASE_E('9',				Alpha9)			// 10)
				CASE_E('A',				A)
				CASE_E('B',				B)
				CASE_E('C',				C)
				CASE_E('D',				D)
				CASE_E('E',				E)
				CASE_E('F',				F)
				CASE_E('G',				G)
				CASE_E('H',				H)
				CASE_E('I',				I)
				CASE_E('J',				J)
				CASE_E('K',				K)
				CASE_E('L',				L)
				CASE_E('M',				M)
				CASE_E('N',				N)
				CASE_E('O',				O)
				CASE_E('P',				P)
				CASE_E('Q',				Q)
				CASE_E('R',				R)
				CASE_E('S',				S)
				CASE_E('T',				T)
				CASE_E('U',				U)
				CASE_E('V',				V)
				CASE_E('W',				W)
				CASE_E('X',				X)
				CASE_E('Y',				Y)
				CASE_E('Z',				Z)
				CASE_E(VK_UP,			UpArrow)
				CASE_E(VK_DOWN,			DownArrow)
				CASE_E(VK_LEFT,			LeftArrow)
				CASE_E(VK_RIGHT,		RightArrow)
				CASE_E(VK_INSERT,		Insert)
				CASE_E(VK_DELETE,		Delete)
				CASE_E(VK_HOME,			Home)
				CASE_E(VK_END,			End)
				CASE_E(VK_PRIOR,		PageUp)
				CASE_E(VK_NEXT,			PageDown)
				CASE_E(VK_NUMPAD0,		Keypad0)
				CASE_E(VK_NUMPAD1,		Keypad1)
				CASE_E(VK_NUMPAD2,		Keypad2)
				CASE_E(VK_NUMPAD3,		Keypad3)
				CASE_E(VK_NUMPAD4,		Keypad4)
				CASE_E(VK_NUMPAD5,		Keypad5)
				CASE_E(VK_NUMPAD6,		Keypad6)
				CASE_E(VK_NUMPAD7,		Keypad7)
				CASE_E(VK_NUMPAD8,		Keypad8)
				CASE_E(VK_NUMPAD9,		Keypad9)
				CASE_E(VK_SNAPSHOT,		PrintScreen)
				CASE_E(VK_SCROLL,		ScrollLock)
				CASE_E(VK_PAUSE,		Pause)
				CASE_E(VK_NUMLOCK,		NumLock)
				CASE_E(VK_DIVIDE,		KeypadDivide)	// /
				CASE_E(VK_MULTIPLY,		KeypadMultiply)	// *
				CASE_E(VK_SUBTRACT,		KeypadMinus)	// -
				CASE_E(VK_ADD,			KeypadPlus)		// +
				CASE_E(VK_DECIMAL,		KeypadPeriod)	// .
			} break;
			#undef CASE_E
		}
		default:
			return false;
	}

	// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-char
	if (ev.charCode > 32) {
		ev.charCodeStr = static_cast<char>(ev.charCode);
	}

	if (ev.keyCode != KeyCode::Ctrl && !BitUtil::hasAny(ev.modifier, Modifier::Ctrl)) {
		_resetModifiedKeyCodeState(_pressedKeyCodes[VK_LCONTROL]);
		_resetModifiedKeyCodeState(_pressedKeyCodes[VK_RCONTROL]);
	}
	if (ev.keyCode != KeyCode::Shift && !BitUtil::hasAny(ev.modifier, Modifier::Shift)) {
		_resetModifiedKeyCodeState(_pressedKeyCodes[VK_LSHIFT]);
		_resetModifiedKeyCodeState(_pressedKeyCodes[VK_RSHIFT]);
	}
	if (ev.keyCode != KeyCode::Alt && !BitUtil::hasAny(ev.modifier, Modifier::Alt)) {
		_resetModifiedKeyCodeState(_pressedKeyCodes[VK_LMENU]);
		_resetModifiedKeyCodeState(_pressedKeyCodes[VK_RMENU]);
	}

	ev.pressedKeyCodes.resize(static_cast<size_t>(KeyCode::_End));
	for (auto k = KeyCode::None + 1; k != KeyCode::_End; k += 1) {
		auto vk = Win32Util::toVKKey(k);
		ev.pressedKeyCodes[enumInt(k)] = _pressedKeyCodes[vk];
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
				auto& si = thisObj->_hScrollInfo;
				int oldPos;
				si->getPos(oldPos);
				int request = LOWORD(wParam);
				switch (request) {
					case SB_LINELEFT:	ev.mode = Mode::Step;   si->scrollTo(oldPos - si->_win32_step());	break;
					case SB_LINERIGHT:	ev.mode = Mode::Step;   si->scrollTo(oldPos + si->_win32_step());	break;
					case SB_PAGELEFT:	ev.mode = Mode::Page;   si->scrollTo(oldPos - si->_win32_page());	break;
					case SB_PAGERIGHT:	ev.mode = Mode::Page;   si->scrollTo(oldPos + si->_win32_page());	break;
					case SB_THUMBTRACK:	ev.mode = Mode::Scroll; si->scrollTo(HIWORD(wParam));				break;
				}
				si->getPos(ev.pos.x);
				ev.deltaPos.x = ev.pos.x - oldPos;
			}
		} break;

		case WM_VSCROLL: {
			if (auto* thisObj = s_getThis(hwnd)) {
				auto& si = thisObj->_vScrollInfo;
				int oldPos;
				si->getPos(oldPos);
				int request = LOWORD(wParam);
				switch (request) {
					case SB_LINEUP:		ev.mode = Mode::Step;   si->scrollTo(oldPos - si->_win32_step());	break;
					case SB_LINEDOWN:	ev.mode = Mode::Step;   si->scrollTo(oldPos + si->_win32_step());	break;
					case SB_PAGEUP:		ev.mode = Mode::Page;   si->scrollTo(oldPos - si->_win32_page());	break;
					case SB_PAGEDOWN:	ev.mode = Mode::Page;   si->scrollTo(oldPos + si->_win32_page());	break;
					case SB_THUMBTRACK:	ev.mode = Mode::Scroll; si->scrollTo(HIWORD(wParam));				break;
				}
				si->getPos(ev.pos.y);
				ev.deltaPos.y = ev.pos.y - oldPos;
			}
		} break;

		default:
			return false;
	}

	if (ev.deltaPos.x || ev.deltaPos.y) {
		ScrollWindow(_hwnd, -ev.deltaPos.x, -ev.deltaPos.y, nullptr, nullptr);
		onUINativeScrollBarEvent(ev);
	}

	return true;
}

LRESULT NativeUIWindow_Win32::_handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (_handleNativeUIMouseEvent(hwnd, msg, wParam, lParam)) return 0;
	if (_handleNativeUIKeyboardEvent(hwnd, msg, wParam, lParam)) return 0;
	if (_handleNativeUIScrollBarEvent(hwnd, msg, wParam, lParam)) return 0;
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

UIEventModifier NativeUIWindow_Win32::_getWin32Modifier() {
	auto o = UIEventModifier::None;
	if (GetAsyncKeyState(VK_CONTROL))	o |= UIEventModifier::Ctrl;
	if (GetAsyncKeyState(VK_SHIFT))		o |= UIEventModifier::Shift;
	if (GetAsyncKeyState(VK_MENU))		o |= UIEventModifier::Alt;
	if (GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN)) {
		o |= UIEventModifier::Cmd;
	}
	return o;
}

bool NativeUIWindow_Win32::isKeyDown(KeyCode keyCode) {
	return _isKeyDown(Win32Util::toVKKey(keyCode));
}

void NativeUIWindow_Win32::_resetModifiedKeyCodeState(UIKeyCodeEventType& keyState) {
	using Type = UIKeyCodeEventType;
	if (keyState == Type::None) return;
	keyState = static_cast<Type>(enumInt(keyState) >> 1);
}

}

#endif // SGE_OS_WINDOWS