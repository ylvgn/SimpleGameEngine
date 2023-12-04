#include <sge_prog_win5.h>

namespace sge {

#define MySampleType_ENUM_LIST(E) \
	E(HelloMsg,) \
	E(ScrnSize,) \
	E(HelloWin,) \
	E(HelloWin2,) \
// ----------
SGE_ENUM_CLASS(MySampleType, u8)
SGE_ENUM_ALL_OPERATOR(MySampleType)

class ProgWin5App : public NativeUIApp {
	using Base = NativeUIApp;
	using Type = MySampleType;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../examples/Test103");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1500, 900 };

			#define E(SGE_E, ...) \
				case Type::SGE_E: { \
					_mainWin = eastl::make_unique<PW5_##SGE_E>(); \
					_mainWin->create(winDesc); \
					_mainWin->setWindowTitle(SGE_STRINGIFY(PW5_, SGE_E)); \
				} break; \
			// ----------

			switch (_sampleType) {
				MySampleType_ENUM_LIST(E)
				default: throw SGE_ERROR("unsupported sample type {}", _sampleType);
			}
			#undef E
		}
	}

private:
	MySampleType			 _sampleType = MySampleType::HelloWin2;
	UPtr<ProgWin5WindowBase> _mainWin;
};

}

int main(int argc, const TCHAR** argv) {
	sge::ProgWin5App app;
	sge::ProgWin5App::CreateDesc appDesc;
	app.run(appDesc);

	return 0;
}

// LPSTR -> Long Pointer String is an artifact of 16-bit Windows,
	// long means 32-bit int, so long pointer is 32-bit pointer;
	// PSTR means 16-bit pointer
// WINAPI/CALLBACK -> __stdcall function calling convention
	// involves how machine code is generated to place function call arguments on the stack

// main entry
	// Win32: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow);
		// HINSTANCE -> instance handle, is simply a number, to uniquely identifies the program.
		// hPrevInstance is always NULL(0) in Win32
	// Win16: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)

// Window message handler
	// Win32: LRESULT CALLBACK WndProc(HWND  , UINT message, WPARAM wParam)
	// Win16: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam)
