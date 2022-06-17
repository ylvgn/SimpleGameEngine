#include "VertexLayoutManager.h"

namespace sge {

VertexLayoutManager* VertexLayoutManager::instance() {
	static VertexLayoutManager s;
	return &s;
}

const VertexLayout* VertexLayoutManager::getLayout(VertexType type) {
	return nullptr;
}

} // namespace