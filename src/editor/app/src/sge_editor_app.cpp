#include <sge_editor.h>

#include <sge_render.h>
#include <sge_render/mesh/RenderMesh.h>
#include <sge_render/mesh/WavefrontObjLoader.h>

#include <sge_core/file/FilePath.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_render/vertex/VertexLayoutManager.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}
#if 1
		auto shader = renderer->createShader("Assets/Shaders/test.shader");
		_material = renderer->createMaterial();
		_material->setShader(shader);
		EditMesh editMesh;

		WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
		// the current shader need color
		for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
			editMesh.color.emplace_back(255, 255, 255, 255);
		}
#else
		auto shader = renderer->createShader("Assets/Shaders/test.hlsl");
		_material = renderer->createMaterial();
		_material->setShader(shader);

		EditMesh editMesh;
		editMesh.pos.emplace_back(0.0f, 0.5f, 0.0f);
		editMesh.pos.emplace_back(0.5f, -0.5f, 0.0f);
		editMesh.pos.emplace_back(-0.5f, -0.5f, 0.0f);

		editMesh.color.emplace_back(255, 0, 0, 255);
		editMesh.color.emplace_back(0, 255, 0, 255);
		editMesh.color.emplace_back(0, 0, 255, 255);

		editMesh.uv[0].emplace_back(0, 1);
		editMesh.uv[0].emplace_back(1, 1);
		editMesh.uv[0].emplace_back(1, 0);

		editMesh.normal.emplace_back(0, 0, 0);
		editMesh.normal.emplace_back(0, 0, 0);
		editMesh.normal.emplace_back(0, 0, 0);

		// because CullMode = D3D11_CULL_BACK; 2->1->0
		editMesh.indices.emplace_back(2);
		editMesh.indices.emplace_back(1);
		editMesh.indices.emplace_back(0);
#endif
		_renderMesh.create(editMesh);
	}

	virtual void onCloseButton() {
		NativeUIApp::current()->quit(0);
	}

	virtual void onDraw() {
		Base::onDraw();
		if (!_renderContext) return;

		auto time = GetTickCount() * 0.001f;
		auto s = abs(sin(time * 2));
		//auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, 0, 0, 1));

		_renderContext->setFrameBufferSize(clientRect().size);

		_renderContext->beginRender();

		_cmdBuf.reset();
		_cmdBuf.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });
		_cmdBuf.drawMesh(SGE_LOC, _renderMesh, _material);
		_cmdBuf.swapBuffers();

		_renderContext->commit(_cmdBuf);

		_renderContext->endRender();

		drawNeeded();
	}

	SPtr<RenderContext>	_renderContext;
	RenderCommandBuffer _cmdBuf;
	RenderMesh _renderMesh;
	SPtr<Material> _material;
};

class EditorApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../examples/Test101");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

	#if 1 // for quick testing
		{ // compile shader
			SHELLEXECUTEINFO ShExecInfo = {};
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = L"open";
			ShExecInfo.lpFile = L"compile_shaders.bat";
			ShExecInfo.lpParameters = L"";
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_SHOW;
			ShExecInfo.hInstApp = NULL;
			ShellExecuteEx(&ShExecInfo);
			WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
			CloseHandle(ShExecInfo.hProcess);
		}
	#endif

		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			//renderDesc.apiType = OpenGL;
			Renderer::create(renderDesc);
		}

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Editor");
		}
	}

private:
	MainWin		_mainWin;
};

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;
	app.run(desc);

	return 0;
}