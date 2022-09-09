#include "NativeImGui.h"
#include <sge_render.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_render/buffer/RenderGpuBuffer.h>

#if SGE_OS_WINDOWS
	#include "imgui_impl_win32.h"
#endif

namespace sge {

NativeImGui::NativeImGui() {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup backend capabilities flags
    //io.BackendRendererUserData = (void*)bd;
    //io.BackendRendererName = "imgui_impl_dx11";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
}

void NativeImGui::create(CreateDesc& desc) {
	ImGui::StyleColorsDark();

#if SGE_OS_WINDOWS
	ImGui_ImplWin32_Init(desc.window);
#endif

	_createDeviceObjects();
	_createFontsTexture();
}

void NativeImGui::destroy() {
#if SGE_OS_WINDOWS
	ImGui_ImplWin32_Shutdown();
#endif
	ImGui::DestroyContext();
}

void NativeImGui::_createFontsTexture() {
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

	// Store our identifier
	//io.Fonts->SetTexID((ImTextureID)bd->pFontTextureView); // ???
}

void NativeImGui::_createDeviceObjects()
{
	auto* renderer = Renderer::instance();

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

void NativeImGui::_renderDrawData(RenderRequest& req) {
	auto* data = ImGui::GetDrawData();
	if (!data) return;

	// Avoid rendering when minimized
	if (data->DisplaySize.x <= 0 || data->DisplaySize.y <= 0)
		return;

	if (data->TotalVtxCount <= 0 || data->TotalIdxCount <= 0)
		return;

	auto* renderer = Renderer::instance();
	
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

bool show_demo_window = true; // test
void NativeImGui::render(RenderRequest& req)
{
#if SGE_OS_WINDOWS
	ImGui_ImplWin32_NewFrame();
#endif
	ImGui::NewFrame();

	if (!_material) { SGE_ASSERT(false); return; }
	req.setMaterialCommonParams(_material);

	_material->setParam("texture0", _fontsTexture);

	{
		// demo
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	// Rendering
	ImGui::Render();
	_renderDrawData(req);
}

} // namespace