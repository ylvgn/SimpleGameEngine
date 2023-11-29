#include <sge_prog_win5.h>

namespace sge {

#define MySampleType_ENUM_LIST(E) \
	E(None,) \
	E(HelloMsg,) \
	E(ScrnSize,) \
	E(HelloWin,) \
	E(_END,) \
// ----------
SGE_ENUM_CLASS(MySampleType, u8)
SGE_ENUM_ALL_OPERATOR(MySampleType)

class ProgWin5App : public NativeUIApp {
	using Base = NativeUIApp;
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

			switch (sampleType)
			{
				case MySampleType::None:
				case MySampleType::_END: break;
				case MySampleType::HelloMsg: {
					_mainWin = eastl::make_unique<PW5_HelloMsg>();
					_mainWin->create(winDesc);
				} break;
				case MySampleType::ScrnSize: {
					_mainWin = eastl::make_unique<PW5_ScrnSize>();
					_mainWin->create(winDesc);
				} break;
				case MySampleType::HelloWin: {
					_mainWin = eastl::make_unique<PW5_HelloWin>();
					_mainWin->create(winDesc);
				} break;
				default: throw SGE_ERROR("unsupported sample type {}", sampleType);
			}
		}
	}

public:
	MySampleType sampleType = MySampleType::None;

private:
	UPtr<ProgWin5WindowBase> _mainWin;
};

}

int main(int argc, const TCHAR** argv) {
	sge::ProgWin5App app;
	app.sampleType = sge::MySampleType::HelloWin; // just for test

	sge::ProgWin5App::CreateDesc desc;
	app.run(desc);

	return 0;
}