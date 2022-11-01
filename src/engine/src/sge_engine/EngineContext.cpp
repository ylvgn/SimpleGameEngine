#include "EngineContext.h"

#include <sge_engine/components/CTransform.h>
#include <sge_engine/components/CMeshRenderer.h>
#include <sge_engine/components/CCamera.h>

namespace sge {

EngineContext* EngineContext::s_instance = nullptr;
RendererSystem* EngineContext::s_rendererSystem = nullptr;

EngineContext* EngineContext::create() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = new EngineContext();
	s_rendererSystem = new RendererSystem();
    return s_instance;
}

void EngineContext::destroy() {
	if (s_instance) {
		delete s_rendererSystem;
		s_rendererSystem = nullptr;

		delete s_instance;
		s_instance = nullptr;
	}
}

EngineContext::EngineContext() {
	registerComponentType<CTransform>();
	registerComponentType<CRenderer>();
	registerComponentType<CMeshRenderer>();
	registerComponentType<CCamera>();
}

const TypeInfo* EngineContext::getComponentType(StrView typeName) {
	TempString name = typeName;
	auto it = _componentTypeMap.find(name.c_str());
	if (it != _componentTypeMap.end()) {
		return it->second;
	}
	return nullptr;
}

void EngineContext::_registerComponentType(const TypeInfo* type) {
	_componentTypeMap.insert(type->name, type);
}

}
