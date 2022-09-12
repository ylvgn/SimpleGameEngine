#include "ImGui_SGE.h"
#include <sge_render/Renderer.h>
#include <sge_render/vertex/Vertex.h>

#if SGE_OS_WINDOWS
	#include "imgui_impl_win32.h"
#endif

namespace sge {

ImGui_SGE::~ImGui_SGE() {
	destroy();
}

void ImGui_SGE::create(CreateDesc& desc) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls

	auto* renderer = Renderer::instance();

	ImGui::StyleColorsDark();

#if SGE_OS_WINDOWS
	ImGui_ImplWin32_Init(desc.window);
#endif

	{ // vertex layout
		Vertex vertex;
		_vertexLayout = vertex.s_layout();
	}

	{ // material
		auto shader = Renderer::instance()->createShader("Assets/Shaders/imgui.shader");
		_material = renderer->createMaterial();
		_material->setShader(shader);
	}
}

void ImGui_SGE::destroy() {
#if SGE_OS_WINDOWS
	ImGui_ImplWin32_Shutdown();
#endif
	ImGui::DestroyContext();
}

void ImGui_SGE::_createFontsTexture() {
	auto* renderer = Renderer::instance();

	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int w, h;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &w, &h);

	Texture2D_CreateDesc texDesc;
	auto& image = texDesc.imageToUpload;
	texDesc.size.set(w, h);
	texDesc.colorType = ColorType::RGBAb;
	texDesc.mipmapCount = 1;
	image.create(Color4b::kColorType, w, h);
	image.copyToPixelData(ByteSpan(reinterpret_cast<const u8*>(pixels), w * h));

	_fontsTexture = renderer->createTexture2D(texDesc);
}

void ImGui_SGE::beginRender(RenderContext* renderContext) {
	if (!_fontsTexture) {
		_createFontsTexture();
	}

#if SGE_OS_WINDOWS
	ImGui_ImplWin32_NewFrame();
#endif

	ImGuiIO& io = ImGui::GetIO();
	auto s = renderContext->frameBufferSize();
	io.DisplaySize = ImVec2(s.x, s.y);
	io.DeltaTime = 1.0f / 60.0f;

	ImGui::NewFrame();
	ImGui::ShowDemoWindow(nullptr); // test demo
}

void ImGui_SGE::render(RenderRequest& req) {
	// Rendering
	ImGui::Render();
	
	if (!_material) return;

	auto* data = ImGui::GetDrawData();
	if (!data) return;

	// Avoid rendering when minimized
	if (data->DisplaySize.x <= 0 || data->DisplaySize.y <= 0)
		return;

	if (data->TotalVtxCount <= 0 || data->TotalIdxCount <= 0)
		return;

	auto* renderer = Renderer::instance();

	{ // material
		if (!_material) { SGE_ASSERT(false); return; }
		req.setMaterialCommonParams(_material);

		float L = data->DisplayPos.x;
		float R = data->DisplayPos.x + data->DisplaySize.x;
		float T = data->DisplayPos.y;
		float B = data->DisplayPos.y + data->DisplaySize.y;
		Mat4f mvp(
			{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f }
		);
		_material->setParam("ProjectionMatrix", mvp);
		_material->setParam("texture0", _fontsTexture);
	}

	auto vertexSize = sizeof(ImDrawVert);
	auto indexSize = sizeof(ImDrawIdx);

	SGE_ASSERT(vertexSize == _vertexLayout->stride);
	SGE_ASSERT(indexSize == sizeof(u16));

	auto totalVertexDataSize = data->TotalVtxCount * vertexSize;
	auto totalIndexDataSize = data->TotalIdxCount * indexSize;

	// vertexbuffer
	if (!_vertexBuffer || _vertexBuffer->bufferSize() < totalVertexDataSize)
	{
		if (_vertexBuffer) {
			_vertexBuffer.reset(nullptr);
		}
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Vertex;
		desc.bufferSize = totalVertexDataSize;
		_vertexBuffer = renderer->createGpuBuffer(desc);
	}

	// indexbuffer
	if (!_indexBuffer || _indexBuffer->bufferSize() < totalIndexDataSize)
	{
		if (_indexBuffer) {
			_indexBuffer.reset(nullptr);
		}
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = totalIndexDataSize;
		_indexBuffer = renderer->createGpuBuffer(desc);
	}

	{
		_vertexData.clear();
		_indexData.clear();
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* src = data->CmdLists[n];

			auto* cmd = req.commandBuffer.newCommand<RenderCommand_DrawCall>();
#if _DEBUG
			cmd->debugLoc = SGE_LOC;
#endif
			cmd->material			= _material;
			cmd->materialPassIndex	= 0;
			cmd->primitive			= RenderPrimitiveType::Triangles;

			cmd->vertexLayout		= _vertexLayout;
			cmd->vertexBuffer		= _vertexBuffer;
			cmd->vertexOffset		= _vertexData.size();
			cmd->vertexCount		= src->VtxBuffer.Size;

			cmd->indexType			= RenderDataTypeUtil::get<ImDrawIdx>();
			cmd->indexBuffer		= _indexBuffer;
			cmd->indexOffset		= _indexData.size();
			cmd->indexCount			= src->IdxBuffer.Size;

			_vertexData.appendRange(Span<const u8>(reinterpret_cast<const u8*>(src->VtxBuffer.Data), src->VtxBuffer.Size * vertexSize));
			 _indexData.appendRange(Span<const u8>(reinterpret_cast<const u8*>(src->IdxBuffer.Data), src->IdxBuffer.Size * indexSize));
		}

		_vertexBuffer->uploadToGpu(_vertexData);
		 _indexBuffer->uploadToGpu(_indexData);
	}
}

void ImGui_SGE::endRender() {
}

} // namespace