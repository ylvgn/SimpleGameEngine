#include <sge_learnopengl.h>

namespace sge {

class sge_learnopengl_example_001 : public JoeyOGL_NativeUIWindow {
	using Base = JoeyOGL_NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();
		auto* editor = EditorContext::instance();

		_shader = renderer->createShader("Assets/Shaders/test.shader");

		{ // create texture
			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker_BC7.dds");
		}

		{ // create entity
			{ // create mesh
				EditMesh editMesh;

				WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
				EditMeshUtil::addColors(editMesh, Color4b(255, 255, 255, 255));

				_meshAsset = new MeshAsset();
				_meshAsset->mesh.create(editMesh);
			}
			auto* e = _scene.addEntity("Entity");
			auto* t = e->transform();
			{ // CMeshRenderer
				auto* mr = e->addComponent<CMeshRenderer>();
				mr->mesh = _meshAsset;
				auto mtl = renderer->createMaterial();
				mtl->setShader(_shader);
				mtl->setParam("test_color", Color4f(1, 1, 1, 1));
				mtl->setParam("test_float", 1.f);
				mtl->setParam("mainTex", _testTexture);
				mr->material = mtl;
			}

			{ // CTransform
				t->setLocalPos(0, 0, 0);
			}

			editor->entitySelection.add(EntityId(1));
		}
	}

	virtual void onDraw() override {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();
		_renderRequest.reset(_renderContext, _camera);

		_renderRequest.cameraFrustum = _camera.frustum();
		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

		CRendererSystem::instance()->render(_renderRequest);

		_hierarchyWindow.draw(_scene, _renderRequest);
		_inspectorWindow.draw(_scene, _renderRequest);
		_statisticsWindow.draw(_scene, _renderRequest);

		_renderContext->drawUI(_renderRequest);
		_renderRequest.swapBuffers();
		_renderRequest.commit();

		_renderContext->endRender();
		drawNeeded();
	}

	SPtr<Texture2D>		_testTexture;
	SPtr<MeshAsset>		_meshAsset;
	SPtr<Shader>		_shader;
};

} // namespace sge

SGE_MAIN(sge::JoeyOGL_NativeUIApp<sge::sge_learnopengl_example_001>)