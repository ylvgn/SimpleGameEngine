#pragma once
#include "RenderCommon.h"

namespace sge {
	// render context
	class RenderContext; // forward declare
	struct RenderContext_CreateDesc;

	// abstract class
	class Renderer : public NonCopyable {
	public:

		enum class ApiType {
			None = 0,
			DX11,
			OpenGL,
		};

		struct CreateDesc {
			CreateDesc(); // declare constructor -> RenderContext::CreateDesc::CreateDesc()
			ApiType apiType;
			bool multithread : 1;
		};

		Renderer();
		virtual ~Renderer();

		static Renderer* current() { return _current; };
		const RenderAdapterInfo& adapterInfo() { return _adapterInfo; };

		static Renderer* create(CreateDesc& desc);
		RenderContext* createRenderContext(RenderContext_CreateDesc& desc) { return onCreateRenderContext(desc); }

	protected:
		static Renderer* _current;
		RenderAdapterInfo _adapterInfo;
		virtual RenderContext* onCreateRenderContext(RenderContext_CreateDesc& desc) = 0;
	};

} // namespace