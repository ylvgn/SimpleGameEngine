#include <sge_learnopengl.h>

namespace sge {

class sge_learnopengl_example_002 : public JoeyOGL_NativeUIWindow {
	using Base = JoeyOGL_NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();
		auto* editor = EditorContext::instance();

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

			Vec3f lightPositions[] = {
				Vec3f(0.0f, 0.0f, 10.0f),
				Vec3f(3.0f, 13.0f, 3.0f),
				Vec3f(0.0f, 3.0f, 0.0f),
				Vec3f(-3.0f, -3.0f, -3.0f),
			};
#if 0 // TODO Color4b in cpu side, float3 in gpu side
			Color4b lightColors[] = {
				Color4b(150, 150, 150, 255),
			};
#else
			float d = 255.f;
			Vec3f lightColors[] = {
				Vec3f(d,   d,   d),
				Vec3f(d,   0.f, 0.f),
				Vec3f(0.f, d,   0.f),
				Vec3f(0.f, 0.f, d),
			};
#endif
			SGE_STATIC_ASSERT(sizeof(lightPositions) / sizeof(lightPositions[0]) == sizeof(lightColors) / sizeof(lightColors[0]));

			auto* e = _scene.addEntity("Sphere");
			auto* t = e->transform();
			t->setLocalPos(0, 0, 0);
			{ // add CMeshRenderer
				auto* mr = e->addComponent<CMeshRenderer>();
				mr->mesh = _meshAsset;
				auto mtl = renderer->createMaterial();
				mtl->setShader(_shader);
#if 1 // test 1 light first, unsupported float3 lightPositions[4] in shader compiler
				mtl->setParam("my_light_positions", lightPositions[0]);
				mtl->setParam("my_light_colors", lightColors[0]);
				mtl->setParam("albedoMap", _albedoMap);
				mtl->setParam("normalMap", _normalMap);
				mtl->setParam("metallicMap", _metallicMap);
				mtl->setParam("roughnessMap", _roughnessMap);
				mtl->setParam("aoMap", _aoMap); // when forget pass one texture will crash TODO
#endif
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