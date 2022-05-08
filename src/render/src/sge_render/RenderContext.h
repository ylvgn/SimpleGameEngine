#pragma once

#include <sge_core/native_ui/NativeUI.h>

namespace sge {
	struct RenderContext_CreateDesc {
		NativeUIWindow* window;
	};

	// abstruct class
	class RenderContext : public NonCopyable {
	public:
		using CreateDesc = RenderContext_CreateDesc;

		RenderContext(CreateDesc& desc);
		virtual ~RenderContext() = default;
		static RenderContext* create(CreateDesc& desc);

		void beginRender()              { onBeginRender(); }
		void endRender()                { onEndRender(); }
		void draw()                     { onDraw(); } // tmp
		void swapBuffers()              { onSwapBuffers(); };
		void clearColorAndDepthBuffer() { onClearColorAndDepthBuffer(); };

		// test
		void render();

	protected:
		virtual void onBeginRender() {};
		virtual void onEndRender() {};
		virtual void onDraw() {};
		virtual void onSwapBuffers() {};
		virtual void onClearColorAndDepthBuffer() {};
	};

} // namespace