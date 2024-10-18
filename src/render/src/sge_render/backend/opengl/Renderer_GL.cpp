#include "Renderer_GL.h"
#include "RenderContext_GL.h"
#include "RenderGpuBuffer_GL.h"
#include "Material_GL.h"
#include "Texture_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

sgeRenderer_InterfaceFunctions_Impl(GL);

Renderer_GL::Renderer_GL(CreateDesc& desc) {
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL