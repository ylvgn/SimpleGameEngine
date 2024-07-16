#include "MyEditMesh.h"

namespace sge {

void MyEditMesh::clear() {
	indices.clear();
	pos.clear();
	for (int i = 0; i < kUvCountMax; ++i) {
		uv[i].clear();
	}
	color.clear();
	normal.clear();
}

}