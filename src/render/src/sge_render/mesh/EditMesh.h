#pragma once

#include <sge_render/Render_Common.h>

namespace sge {

#if 0
#pragma mark ========= EditMeshUtil ============
#endif
struct EditMeshUtil {
	EditMeshUtil() = delete;

	static void addColors(EditMesh& src, const Color4b& solidColor);

	static Span<const u32> subIndices(const EditMesh& src, size_t count, size_t offset = 0);
	static Span<const Tuple3f> subPos(const EditMesh& src, size_t count, size_t offset = 0);
}; // EditMeshUtil


#if 0
#pragma mark ========= EditMeshUtil ============
#endif
class EditMesh : public NonCopyable {
	using Util = EditMeshUtil;
public:

	static const u8 kUvCountMax = 4;

	RenderPrimitiveType	primitive = RenderPrimitiveType::Triangles;
	Vector<u32>	indices;

	Vector<Tuple3f>	pos;
	Vector<Tuple2f>	uv[kUvCountMax];
	Vector<Color4b> color;

	Vector<Tuple3f>	normal;
	Vector<Tuple3f>	tangent;
	Vector<Tuple3f>	binormal;

	void clear();

	void onFormat(fmt::format_context& ctx) const;
}; // EditMesh

SGE_FORMATTER(EditMesh)

} // namespace sge