#include "Win32Util.h"
#include <sge_core/input/UIEvent.h>

namespace sge {

int Win32Util::toVKKey(const KeyCode& i) {
	#define CASE_E(SGE_T, WIN32_VK_T) case KeyCode::SGE_T: return WIN32_VK_T;

	switch (i) {
		CASE_E(LCtrl,	VK_LCONTROL)
		CASE_E(RCtrl,	VK_RCONTROL)
		CASE_E(LShift,	VK_LSHIFT)
		CASE_E(RShift,	VK_RSHIFT)
		CASE_E(LAlt,	VK_LMENU)
		CASE_E(RAlt,	VK_RMENU)
		CASE_E(LCmd,	VK_LWIN)
		CASE_E(RCmd,	VK_RWIN)

		CASE_E(Enter,		VK_RETURN)
		CASE_E(Escape,		VK_ESCAPE)
		CASE_E(Tab,			VK_TAB)
		CASE_E(CapsLock,	VK_CAPITAL)
		CASE_E(Space,		VK_SPACE)
		CASE_E(Backspace,	VK_BACK)

		CASE_E(Semicolon,		VK_OEM_1)		// ;:
		CASE_E(Slash,			VK_OEM_2)		// /?
		CASE_E(BackQuote,		VK_OEM_3)		// `~
		CASE_E(LeftBracket,		VK_OEM_4)		// [{
		CASE_E(Backslash,		VK_OEM_5)		// \|
		CASE_E(RightBracket,	VK_OEM_6)		// ]}
		CASE_E(Quote,			VK_OEM_7)		// '~
		CASE_E(Equals,			VK_OEM_PLUS)	// =+
		CASE_E(Hyphen,			VK_OEM_MINUS)	// -_
		CASE_E(Comma,			VK_OEM_COMMA)	// ,<
		CASE_E(Period,			VK_OEM_PERIOD)	// .>

		CASE_E(F1,	VK_F1)
		CASE_E(F2,	VK_F2)
		CASE_E(F3,	VK_F3)
		CASE_E(F4,	VK_F4)
		CASE_E(F5,	VK_F5)
		CASE_E(F6,	VK_F6)
		CASE_E(F7,	VK_F7)
		CASE_E(F8,	VK_F8)
		CASE_E(F9,	VK_F9)
		CASE_E(F10, VK_F10)
		CASE_E(F11, VK_F11)
		CASE_E(F12, VK_F12)

		CASE_E(Alpha0, '0')	// 1!
		CASE_E(Alpha1, '1')	// 2@
		CASE_E(Alpha2, '2')	// 3#
		CASE_E(Alpha3, '3')	// 4$
		CASE_E(Alpha4, '4')	// 5%
		CASE_E(Alpha5, '5')	// 6^
		CASE_E(Alpha6, '6')	// 7&
		CASE_E(Alpha7, '7')	// 8*
		CASE_E(Alpha8, '8')	// 9(
		CASE_E(Alpha9, '9')	// 10)

		CASE_E(A, 'A')
		CASE_E(B, 'B')
		CASE_E(C, 'C')
		CASE_E(D, 'D')
		CASE_E(E, 'E')
		CASE_E(F, 'F')
		CASE_E(G, 'G')
		CASE_E(H, 'H')
		CASE_E(I, 'I')
		CASE_E(J, 'J')
		CASE_E(K, 'K')
		CASE_E(L, 'L')
		CASE_E(M, 'M')
		CASE_E(N, 'N')
		CASE_E(O, 'O')
		CASE_E(P, 'P')
		CASE_E(Q, 'Q')
		CASE_E(R, 'R')
		CASE_E(S, 'S')
		CASE_E(T, 'T')
		CASE_E(U, 'U')
		CASE_E(V, 'V')
		CASE_E(W, 'W')
		CASE_E(X, 'X')
		CASE_E(Y, 'Y')
		CASE_E(Z, 'Z')

		CASE_E(UpArrow,		VK_UP)
		CASE_E(DownArrow,	VK_DOWN)
		CASE_E(LeftArrow,	VK_LEFT)
		CASE_E(RightArrow,	VK_RIGHT)

		CASE_E(Insert,		VK_INSERT)
		CASE_E(Delete,		VK_DELETE)
		CASE_E(Home,		VK_HOME)
		CASE_E(End,			VK_END)
		CASE_E(PageUp,		VK_PRIOR)
		CASE_E(PageDown,	VK_NEXT)

		CASE_E(Keypad0, VK_NUMPAD0)
		CASE_E(Keypad1, VK_NUMPAD1)
		CASE_E(Keypad2, VK_NUMPAD2)
		CASE_E(Keypad3, VK_NUMPAD3)
		CASE_E(Keypad4, VK_NUMPAD4)
		CASE_E(Keypad5, VK_NUMPAD5)
		CASE_E(Keypad6, VK_NUMPAD6)
		CASE_E(Keypad7, VK_NUMPAD7)
		CASE_E(Keypad8, VK_NUMPAD8)
		CASE_E(Keypad9, VK_NUMPAD9)

		CASE_E(PrintScreen,		VK_SNAPSHOT)
		CASE_E(ScrollLock,		VK_SCROLL)
		CASE_E(Pause,			VK_PAUSE)
		CASE_E(NumLock,			VK_NUMLOCK)
		CASE_E(KeypadDivide,	VK_DIVIDE)	 // /
		CASE_E(KeypadMultiply,	VK_MULTIPLY) // *
		CASE_E(KeypadMinus,		VK_SUBTRACT) // -
		CASE_E(KeypadPlus,		VK_ADD)		 // +
		CASE_E(KeypadPeriod,	VK_DECIMAL)	 // .
	}

	#undef CASE_E
	return 0;
}

} // namespace