#include <sge_prog_win5.h>

namespace sge {

#define MySampleType_ENUM_LIST(E) \
	/* Chap01 */	\
	E(HelloMsg,)	\
	E(HelloMsg2,)	\
	/* Chap02 */	\
	E(ScrnSize,)	\
	/* Chap03 */	\
	E(HelloWin,)	\
	E(HelloWin2,)	\
// ----------
SGE_ENUM_CLASS(MySampleType, u32)
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
			winDesc.isMainWindow	= true;
			winDesc.centerToScreen	= true;
			winDesc.rect = { 10, 10, 800, 600 };

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