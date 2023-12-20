#include <sge_prog_win5.h>

namespace sge {

#define MySampleType_ENUM_LIST(E) \
	/* Chap01 MessageBox */ \
	E(HelloMsg,)	\
	E(HelloMsg2,)	\
	/* Chap02 Unicode */ \
	E(ScrnSize,)	\
	/* Chap03 CreateWindow and WndProc */ \
	E(HelloWin,)	\
	E(HelloWin2,)	\
	E(HelloWin3,)	\
	/* Chap04 Text Output */	\
	E(SysMets1,)	\
// ----------
SGE_ENUM_CLASS(MySampleType, u32)
SGE_ENUM_ALL_OPERATOR(MySampleType)

#if SGE_OS_WINDOWS

class ProgWin5App : public NativeUIApp_Base {
	using Base = NativeUIApp_Base;
	using Type = MySampleType;

	MySampleType _sampleType = MySampleType::HelloWin3;

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
			winDesc.isMainWindow	= true;
			winDesc.centerToScreen	= true;
			winDesc.rect			= { 10, 10, 800, 600 };

			#define E(SGE_E, ...) \
				case Type::SGE_E: { \
					_mainWin = eastl::make_unique<PW5_##SGE_E>(); \
					_mainWin->create(winDesc); \
					_mainWin->setWindowTitle(SGE_STRINGIFY(PW5_, SGE_E)); \
				} break; \
			// ----------
			switch (_sampleType) {
				MySampleType_ENUM_LIST(E)
				default:
					throw SGE_ERROR("unsupported sample type {}", _sampleType);
			}
			#undef E
		}
	}

	virtual void onRun() override {
		Base::onRun();

		// message loop
		while (GetMessage(&_msg, NULL, 0, 0)) // the 2,3,4 arguments is NULL means wants all messages for all windows created by the program
		{
			TranslateMessage(&_msg); // Translates some keyboard messages
			DispatchMessage(&_msg);  // Sends a message to a window procedure(WndProc)
		}
		// when _msg.message == WM_QUIT, GetMessage will break the loop

		willQuit();
	}

	virtual void onQuit() override {
		Base::onQuit();
		::PostQuitMessage(_exitCode);
	}

private:
	MSG _msg;
	UPtr<ProgWin5WindowBase> _mainWin;
};

}

#endif // SGE_OS_WINDOWS

int main(int argc, const TCHAR** argv) {
	sge::ProgWin5App app;
	sge::ProgWin5App::CreateDesc appDesc;
	app.run(appDesc);

	return 0;
}