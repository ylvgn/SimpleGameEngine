#include "EditMesh.h"

namespace sge {

#if 0
#pragma mark ========= EditMeshUtil ============
#endif
void EditMeshUtil::addColors(EditMesh& src, const Color4b& solidColor) {
	for (size_t i = src.color.size(); i < src.pos.size(); ++i) {
		src.color.emplace_back(solidColor);
	}
}

Span<const Tuple3f> EditMeshUtil::subPos(const EditMesh& src, size_t offset, size_t count) {
	using T = decltype(src.pos)::value_type;

	const T* p = src.pos.begin() + offset;
	if (p + count > src.pos.end()) {
		throw SGE_ERROR("out of range");
	}
	return Span<const T>(p, count);
}

Span<const u32> EditMeshUtil::subIndices(const EditMesh& src, size_t offset, size_t count) {
	using T = decltype(src.indices)::value_type;

	const T* p = src.indices.begin() + offset;
	if (p + count > src.indices.end()) {
		throw SGE_ERROR("out of range");
	}
	return Span<const T>(p, count);
}

#if 0
#pragma mark ========= EditMesh ============
#endif
void EditMesh::clear() {
	indices.clear();
	pos.clear();
	for (int i = 0; i < kUvCountMax; i++) {
		uv[i].clear();
	}
	color.clear();
	normal.clear();
	tangent.clear();
	binormal.clear();
}

void EditMesh::onFormat(fmt::format_context& ctx) const {
	TempString tmp = "";
	if (pos.size()) {
		for (auto& _pos : pos) {
			FmtTo(tmp, "v {} {} {}\n", _pos.x, _pos.y, _pos.z);
		}
	}

	if (uv->size() && uv[0].size()) {
		for (auto& _uv : uv[0]) {
			FmtTo(tmp, "vt {} {}\n", _uv.x, _uv.y);
		}
	}

	if (normal.size()) {
		for (auto& nl : normal) {
			FmtTo(tmp, "vn {} {} {}\n", nl.x, nl.y, nl.z);
		}
	}

	if (indices.size()) {
		for (size_t i = 0; i < indices.size(); ++i) {
			if (i % 3 == 0 && i) {
				size_t j = i - 1;
				FmtTo(tmp, "f {} {} {}\n", indices[j-2], indices[j-1], indices[j]);
			}
		}
	}
	fmt::format_to(ctx.out(), "{}", tmp);
}

}