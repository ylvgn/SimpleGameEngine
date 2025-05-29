#include <sge_learnopengl.h>

namespace sge {

// FYI: https://learnopengl.com/Guest-Articles/2022/Compute-Shaders/Introduction

#define TEST_EXAMPLE_1 1 // !!<--- please keep same macro choose in test_compute_triangles.shader
#define TEST_EXAMPLE_2 0

class sge_learnopengl_example_003 : public JoeyOGL_NativeUIWindow {
	using Base = JoeyOGL_NativeUIWindow;
	using Vertex = Vertex_PosColor;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		_camera.setProjectionType(Math::CameraProjectionType::Orthogrphic);
		_camera.setNearClip(0.01f);
		_camera.setFarClip(1000.0f);
		_camera.setPos(0, 0, 5.f);
		_camera.setAim(0, 0, 0);
		_camera.setUp(0, 1, 0);

		{ // create shader
			_computeShader = renderer->createShader("Assets/Shaders/test_compute_triangles.shader");
			_renderShader = renderer->createShader("Assets/Shaders/test_compute_triangles_display.shader");
		}
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		using KeyCode = UIKeyboardEventKeyCode;
		Base::onUIKeyboardEvent(ev);
	}

	virtual void onDraw() override {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());
		auto clientSize = _camera.viewport().size;

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();
		_renderRequest.reset(_renderContext, _camera);

		_renderRequest.cameraFrustum = _camera.frustum();
		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

		//==== test render triangles from compute shader ===
		auto* renderer = Renderer::instance();

		Tuple2i gridSize(1, 1);
#if TEST_EXAMPLE_1
		int vertexCount = 3;
#elif TEST_EXAMPLE_2
		int vertexCount = gridSize.x * gridSize.y * 6;
#endif

		if (!_computeOutVertexBuffer) {
			{ // create storage buffer
				RenderGpuStorageBuffer_CreateDesc bufDesc;
				bufDesc.bufferSize = vertexCount * sizeof(Vertex);
				bufDesc.stride	   = sizeof(Vertex);
				_computeOutVertexBuffer = renderer->createGpuStorageBuffer(bufDesc);
			}
		}
		if (!_computeMtl) {
			{ // create material
				_computeMtl = renderer->createMaterial();
				_computeMtl->setShader(_computeShader);
#if TEST_EXAMPLE_2
				_computeMtl->setParam("cellSize", 1.0f);
				_computeMtl->setParam("gridSize", gridSize);
#endif
				_computeMtl->setParam("outVertexBuffer", _computeOutVertexBuffer);

				_renderMtl = renderer->createMaterial();
				_renderMtl->setShader(_renderShader);
			}
		}

		{ // add compute call
			auto* cmd			   = _renderRequest.addDrawCall();
#if _DEBUG
			cmd->debugLoc		   = SGE_LOC;
#endif
			cmd->material		   = _computeMtl;
			cmd->materialPassIndex = 0;
			cmd->threadGroupCount.emplace(Tuple3i(1, 1, 1));
		}

		{ // add draw call
			_renderMtl->setParam("color", Color4f(1, 1, 1, 1));

			auto* cmd			   = _renderRequest.addDrawCall();
#if _DEBUG
			cmd->debugLoc		   = SGE_LOC;
#endif
			cmd->primitive		   = RenderPrimitiveType::Triangles;
			cmd->vertexLayout	   = Vertex::s_layout();
			cmd->material		   = _renderMtl;
			cmd->materialPassIndex = 0;
			cmd->vertexCount	   = vertexCount;
			cmd->vertexOffset	   = 0;
			cmd->indexCount		   = 0;
			cmd->vertexBuffer	   = _computeOutVertexBuffer->gpuBuffer();
		}

		_renderContext->drawUI(_renderRequest);
		_renderRequest.swapBuffers();
		_renderRequest.commit();

		_renderContext->endRender();
		drawNeeded();
	}

	SPtr<Shader>				 _computeShader;
	SPtr<Material>				 _computeMtl;
	SPtr<RenderGpuStorageBuffer> _computeOutVertexBuffer;

	SPtr<Shader>		_renderShader;
	SPtr<Material>		_renderMtl;
};

} // namespace sge

SGE_MAIN(sge::JoeyOGL_NativeUIApp<sge::sge_learnopengl_example_003>)