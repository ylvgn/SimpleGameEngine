#include <sge_editor.h>

#include <sge_render.h>
#include <sge_render/mesh/RenderMesh.h>
#include <sge_render/mesh/WavefrontObjLoader.h>
#include <sge_render/mesh/RenderTerrain.h>

#include <sge_core/file/FilePath.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_render/vertex/VertexLayoutManager.h>

#include <sge_render/command/RenderRequest.h>

#include <sge_render/ImGui_SGE.h>

#include <sge_editor/ecs/GameObject.h>
#include <sge_editor/ecs/Transform.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{ // ecs
			for (size_t i = 0; i < 10; i++) {
				SPtr<GameObject> obj = new GameObject();
				Transform* t = obj->addComponent<Transform>();

				Vec3f v3;
				for (int j = 0; j < 3; j++) v3[j] = 1.0f + std::rand() % 99;;
				t->localPosition.set(v3);

				for (int j = 0; j < 3; j++) v3[j] = 10.0f + std::rand() % 99;;
				t->localScale.set(v3);

				Vec4f v4;
				for (int j = 0; j < 4; j++) v4[j] = 0.0f + std::rand() % 99;
				t->localRotation.set(v4.x, v4.y, v4.z, v4.w);

				SGE_DUMP_VAR(i, *t);
				objs.emplace_back(std::move(obj));
			}
		}

		{ // test create primitive
			const TypeInfo* ti = TypeManager::instance()->getType("u8");
			auto t = ti->createPod<u8>();

			SGE_DUMP_VAR(t);
			t = 101;
			SGE_DUMP_VAR(t);
		}

		{ // test create struct
			const TypeInfo* ti = TypeManager::instance()->getType("Vec3f");
			auto t = ti->createPod<Vec3f>();

			SGE_DUMP_VAR(t.x,  t.y, t.z);
			t.set(1, 2, 3);
			SGE_DUMP_VAR(t.x, t.y, t.z);
		}

		{ // test create GameObject
			const TypeInfo* ti = TypeManager::instance()->getType("GameObject");
			if (!ti) {
				ti = TypeManager::instance()->registerType<GameObject>();
			}
			auto obj = ti->createObject<GameObject>();

			Transform* t = obj->addComponent<Transform>();
			t->localPosition.set(999.0f, 99.0f, 9.0f);
			objs.emplace_back(std::move(obj));
		}

		{ // test create Transform
			const TypeInfo* ti = TypeManager::instance()->getType("Transform");
			if (!ti) {
				ti = TypeManager::instance()->registerType<Transform>();
			}
			auto* obj = objs.back().ptr();
			Transform* t = ti->createObject<Transform>(obj);
			obj->addComponent(t);

			t->localPosition.set(888.0f, 88.0f, 8.0f);
		}

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

		{ // imgui
			ImGui_SGE_CreateDesc imguiDesc;
			imguiDesc.window = this;
			_imgui.create(imguiDesc);
		}
#if 1
		_camera.setPos(0, 10, 10);
		//_camera.setPos(0, 1200, 10);  // debug
		_camera.setAim(0, 0, 0);
#else
		// just for test 5x5 terrain 
		_camera.setPos(58.932793f, 38.021767f, 3.6692433f);
		_camera.setAim(0.79875153f, 0.8193707f, 1.8785787f);
#endif

		Texture2D_CreateDesc texDesc;
		auto& image = texDesc.imageToUpload;
#if 1
		//image.loadFile("Assets/Textures/uvChecker.png");
		//image.loadFile("Assets/Textures/uvChecker_BC1.dds");
		//image.loadFile("Assets/Textures/uvChecker_BC2.dds");
		//image.loadFile("Assets/Textures/uvChecker_BC3.dds");
		image.loadFile("Assets/Textures/uvChecker_BC7.dds");

		texDesc.size = image.size();
		texDesc.colorType = image.colorType();
#else
		int w = 256;
		int h = 256;

		texDesc.size.set(w, h);
		texDesc.colorType = ColorType::RGBAb;

		image.create(Color4b::kColorType, w, h);

		for (int y = 0; y < w; y++) {
			auto span = image.row<Color4b>(y);
			for (int x = 0; x < h; x++) {
				span[x] = Color4b(static_cast<u8>(x),	// r, span[x] means row[x]
					static_cast<u8>(y),					// g
					0,									// b
					255);								// a
			}
		}
#endif
		_testTexture = renderer->createTexture2D(texDesc);

		auto shader = renderer->createShader("Assets/Shaders/test.shader");

		_material = renderer->createMaterial();
		_material->setShader(shader);
		_material->setParam("mainTex", _testTexture);

		EditMesh editMesh;
#if 1
		WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
		// the current shader need color
		for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
			editMesh.color.emplace_back(255, 255, 255, 255);
		}
#else
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

		{
			float size = 2048;
			float pos = size / -2;
			float y = -100;
			float height = 200;
			int maxLod = 6;
			_terrain.createFromHeightMapFile(
				Vec3f(pos, y, pos),
				Vec2f(size, size),
				height,
				maxLod,
				"Assets/Terrain/TerrainTest/TerrainHeight_Small.png");
		}
	}

	virtual void onCloseButton() {
		NativeUIApp::current()->quit(0);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		_imgui.onUIMouseEvent(ev);
		if (_imgui.wantCaptureMouse()) return;

		if (ev.isDragging()) {
			using Button = UIMouseEventButton;
			switch (ev.pressedButtons) {
				case Button::Left: {
					auto d = ev.deltaPos * 0.01f;
					_camera.orbit(d.x, d.y);
				}break;

				case Button::Middle: {
					auto d = ev.deltaPos * 0.05f;
					_camera.move(d.x, d.y, 0);
				}break;

				case Button::Right: {
					auto d = ev.deltaPos * -0.05f;
					_camera.dolly(d.x + d.y);
				}break;
			}
			//SGE_LOG("{}\t{}", _camera.pos(), _camera.aim());
		}
	}

	virtual void onUIMouseCursor(UIMouseEvent& ev) override {
		_imgui.onUIMouseCursor(ev);
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		_imgui.onUIKeyboardEvent(ev);
	}

	void drawUI(u8* p, const TypeInfo* ti, const FieldInfo* fi = nullptr) {
		if (ti->isStruct() || ti->isObject()) {
			ImGui::Text(ti->name);
			for (const auto& field : ti->fieldArray) {
				drawUI(p + field.offset, field.fieldInfo, &field);
			}
			return;
		}

		if (ti->isContainer()) {
			// TODO
		}

		if (ti == TypeManager::instance()->getType("u8") || ti == TypeManager::instance()->getType("i8")
			|| ti == TypeManager::instance()->getType("u16") || ti == TypeManager::instance()->getType("i16")
			|| ti == TypeManager::instance()->getType("u32") || ti == TypeManager::instance()->getType("i32")
			|| ti == TypeManager::instance()->getType("u64") || ti == TypeManager::instance()->getType("i64"))
		{
			ImGui::InputInt(fi->name, reinterpret_cast<i32*>(p));
		}
		else if (ti == TypeManager::instance()->getType("f32"))
		{
			ImGui::InputFloat(fi->name, reinterpret_cast<f32*>(p));
		}
		else if (ti == TypeManager::instance()->getType("f64")
			|| ti == TypeManager::instance()->getType("f128"))
		{
			ImGui::InputDouble(fi->name, reinterpret_cast<f64*>(p));
		}
	}

	virtual void onDraw() {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();
		_imgui.beginRender(_renderContext);

		_renderRequest.reset();
		_renderRequest.matrix_model = Mat4f::s_identity();
		_renderRequest.matrix_view = _camera.viewMatrix();
		_renderRequest.matrix_proj = _camera.projMatrix();
		_renderRequest.camera_pos = _camera.pos();

		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

//-----
//		auto time = GetTickCount() * 0.001f;
//		auto s = abs(sin(time * 2));
		auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));
//-----

		_terrain.render(_renderRequest);

		_renderRequest.drawMesh(SGE_LOC, _renderMesh, _material);

#if 0 // imgui
		ImGui::ShowDemoWindow(nullptr);
#else

		static int selected_index = -1;
		// Hierarchy
		ImGui::Begin("Hierarchy", 0, 0);
		{
			int node_clicked = -1;
			static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			static int selection_mask = 0;
			for (int i = 0; i < objs.size(); i++)
			{
				ImGuiTreeNodeFlags node_flags = base_flags;
				const bool is_selected = (selection_mask & (1 << i)) != 0;
				if (is_selected) {
					node_flags |= ImGuiTreeNodeFlags_Selected;
					selected_index = i;
				}
				
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "GameObject %d", i);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
					node_clicked = i;
				}
			}

			if (node_clicked != -1)
			{
				// Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if (ImGui::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
			}
		}
		ImGui::End();

		// Inspector
		ImGui::Begin("Inspector", 0, 0);
		{
			if (selected_index != -1) {
				const auto& obj = objs[selected_index].ptr();
				ImGui::Text("GameObject %d", selected_index);
				for (auto& c : obj->components()) {
					const auto* ti = c->getType();
					drawUI(reinterpret_cast<u8*>(c.get()), ti);
					//ImGui::Text("===============");
				}
				ImGui::NewLine();
			}
		}
		ImGui::End();

#endif
		_imgui.render(_renderRequest);

		_renderRequest.swapBuffers();

		_renderContext->commit(_renderRequest.commandBuffer);

		_renderContext->endRender();
		_imgui.endRender();
		drawNeeded();
	}

	Vector<SPtr<GameObject>> objs;

	ImGui_SGE _imgui;

	RenderTerrain _terrain;

	SPtr<Material> _material;
	SPtr<Texture2D> _testTexture;

	SPtr<RenderContext>	_renderContext;
	RenderMesh _renderMesh;

	Math::Camera3f	_camera;

	RenderRequest	_renderRequest;
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