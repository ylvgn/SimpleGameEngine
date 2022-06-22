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
		layout->stride = sizeof(VERTEX);
		VERTEX::onRegister(layout);
	}

private:
	VertexLayout* _createLayout(VertexType type);
	Map<VertexType, VertexLayout> _table;

}; // VertexLayoutManager

} // namespace