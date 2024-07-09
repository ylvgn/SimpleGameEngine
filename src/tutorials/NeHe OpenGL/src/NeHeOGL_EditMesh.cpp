#include "NeHeOGL_EditMesh.h"

namespace sge {

void NeHeOGL_EditMesh::clear() {
	indices.clear();
	pos.clear();
	for (int i = 0; i < kUvCountMax; ++i) {
		uv[i].clear();
	}
	color.clear();
	normal.clear();
}

}