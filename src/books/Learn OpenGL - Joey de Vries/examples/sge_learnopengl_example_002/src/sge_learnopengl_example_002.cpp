#include <sge_learnopengl.h>

namespace sge {

class sge_learnopengl_example_002 : public JoeyOGL_NativeUIWindow {
	using Base = JoeyOGL_NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();
		auto* editor = EditorContext::instance();

		_camera.setPos(0, 0, 10);
		_camera.setAim(0, 0, 0);

		{ // create shader
			_shader = renderer->createShader("Assets/Shaders/pbr.shader");
		}

		{ // create texture
			_albedoMap    = renderer->createTexture2DFromFile("Assets/Textures/pbr/rusted_iron/albedo.png");
			_normalMap    = renderer->createTexture2DFromFile("Assets/Textures/pbr/rusted_iron/normal.png");
			_metallicMap  = renderer->createTexture2DFromFile("Assets/Textures/pbr/rusted_iron/metallic.png");
			_roughnessMap = renderer->createTexture2DFromFile("Assets/Textures/pbr/rusted_iron/roughness.png");
			_aoMap		  = renderer->createTexture2DFromFile("Assets/Textures/pbr/rusted_iron/ao.png");
		}

		{ // create entity
			{ // create mesh
				EditMesh editMesh;

				WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/sphere.obj");
				EditMeshUtil::addColors(editMesh, Color4b(255, 255, 255, 255));

				_meshAsset = new MeshAsset();
				_meshAsset->mesh.create(editMesh);
			}

			auto* e = _scene.addEntity("My Sphere");
			auto* t = e->transform();
			t->setLocalPos(0, 1.5f, 0);
			{ // add CMeshRenderer
				auto* mr = e->addComponent<CMeshRenderer>();
				mr->mesh = _meshAsset;
				auto mtl = renderer->createMaterial();
				mtl->setShader(_shader);
				// TODO: when forget pass anyone texture will throw exception
				// TODO: cpu Color4b to float3 HLSL
				// TODO: shader compiler reflection const buffer(uniform variable) array vector like float3[]
				mtl->setParam("albedoMap", _albedoMap);
				mtl->setParam("normalMap", _normalMap);
				mtl->setParam("metallicMap", _metallicMap);
				mtl->setParam("roughnessMap", _roughnessMap);
				mtl->setParam("aoMap", _aoMap);
				mtl->setParam("my_switch_impl", 1);
				mr->material = mtl;
			}
		}

		{
			auto* e = _scene.addEntity("Learn OpenGL Sphere");
			auto* t = e->transform();
			t->setLocalPos(0, -1.5f, 0);
			{ // add CMeshRenderer
				auto* mr = e->addComponent<CMeshRenderer>();
				mr->mesh = _meshAsset;
				auto mtl = renderer->createMaterial();
				mtl->setShader(_shader);
				mtl->setParam("albedoMap", _albedoMap);
				mtl->setParam("normalMap", _normalMap);
				mtl->setParam("metallicMap", _metallicMap);
				mtl->setParam("roughnessMap", _roughnessMap);
				mtl->setParam("aoMap", _aoMap);
				mr->material = mtl;
			}
		}

		editor->entitySelection.add(EntityId(0));
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		using KeyCode = UIKeyboardEventKeyCode;
		Base::onUIKeyboardEvent(ev);
	}

	virtual void onDraw() override {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();
		_renderRequest.reset(_renderContext, _camera);

		_renderRequest.cameraFrustum = _camera.frustum();
		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.f, 1 });

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

	SPtr<MeshAsset>		_meshAsset;
	SPtr<Texture2D>		_albedoMap;
	SPtr<Texture2D>		_normalMap;
	SPtr<Texture2D>		_metallicMap;
	SPtr<Texture2D>		_roughnessMap;
	SPtr<Texture2D>		_aoMap;
	SPtr<Shader>		_shader;
};

} // namespace sge

SGE_MAIN(sge::JoeyOGL_NativeUIApp<sge::sge_learnopengl_example_002>)