#include <sge_prog_win5.h>

namespace sge {

#define MySampleType_ENUM_LIST(E)			\
	/* Chap01 MessageBox */					\
	E(HelloMsg,)							\
	E(MyMessageBox,)						\
	/* Chap02 Unicode */					\
	E(ScrnSize,)							\
	/* Chap03 CreateWindow and WndProc */	\
	E(HelloWin,)							\
	E(MyHelloWin,)							\
	/* Chap04 Client Rect and Scroll Bar */ \
	E(MyDrawText,)							\
	E(MyTextMetrics,)						\
	E(SysMets1,)							\
	E(SysMets2,)							\
	E(MySysMets2,)							\
// ----------
SGE_ENUM_CLASS(MySampleType, u32)
SGE_ENUM_ALL_OPERATOR(MySampleType)

#if SGE_OS_WINDOWS

class ProgWin5MainWin : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = ProgWin5MainWin;
	using Type = MySampleType;

	Type _sampleType = Type::MySysMets2;
	UPtr<PW5_NativeUIWindow> _sampleWindow;

public:
	virtual void onCreate(CreateDesc& desc) override {
		#define E(SGE_E, ...) \
		case Type::SGE_E: { \
			_sampleWindow = eastl::make_unique<PW5_##SGE_E>(); \
			_sampleWindow->create(desc); \
			_sampleWindow->setWindowTitle(SGE_STRINGIFY(PW5_, SGE_E)); \
		} break; \
		// ----------
		switch (_sampleType) {
			MySampleType_ENUM_LIST(E)
			default: throw SGE_ERROR("unsupported sample type {}", _sampleType);
		}
		#undef E
	}
};

class ProgWin5App : public PW5_NativeUIApp {
	using Base = PW5_NativeUIApp;
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
			_mainWin.create(winDesc);
		}
	}

private:
	ProgWin5MainWin _mainWin;
};

}

#endif // SGE_OS_WINDOWS

int main(int argc, const TCHAR** argv) {
	sge::ProgWin5App app;
	sge::ProgWin5App::CreateDesc appDesc;
	app.run(appDesc);

	return 0;
}