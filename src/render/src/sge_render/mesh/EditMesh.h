#pragma once

#include <sge_render/Render_Common.h>

namespace sge {

#if 0
#pragma mark ========= EditMeshUtil ============
#endif
struct EditMeshUtil {
	EditMeshUtil() = delete;

	static void addColors(EditMesh& src, const Color4b& solidColor);

	static Span<const u32> subIndices(const EditMesh& src, size_t offset, size_t count);
	static Span<const Tuple3f> subPos(const EditMesh& src, size_t offset, size_t count);
};

#if 0
#pragma mark ========= EditMeshUtil ============
#endif
class EditMesh : public NonCopyable {
public:
	using Util = EditMeshUtil;

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
};

SGE_FORMATTER(EditMesh)

}