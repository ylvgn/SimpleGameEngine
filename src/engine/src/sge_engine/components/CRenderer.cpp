#include "CRenderer.h"

namespace sge {

const TypeInfo* CRenderer::s_getType() {
	class Ti : public CRenderer::TI_Base {
	public:
		Ti() {

		}
	};
	static Ti ti;
	return &ti;
}

}