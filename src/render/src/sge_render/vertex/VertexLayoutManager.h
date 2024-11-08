#pragma once

#include "Vertex.h"

namespace sge {

class VertexLayoutManager {
public:
	VertexLayoutManager();
	static VertexLayoutManager* instance();
	const VertexLayout* getLayout(VertexType type);

	template<class VERTEX>
	void registerLayout() {
		VertexLayout* layout = _createLayout(VERTEX::kType);
		layout->type = VERTEX::kType;
#if 0 // ImGui_SGE not support now, but how ???
		layout->stride = Math::nextPow16(sizeof(VERTEX));
		SGE_ASSERT(layout->stride % 16 == 0);
#else
		layout->stride = sizeof(VERTEX);
#endif
		VERTEX::onRegister(layout);
	}

private:
	VertexLayout* _createLayout(VertexType type);
	Map<VertexType, VertexLayout> _table;

}; // VertexLayoutManager

} // namespace sge