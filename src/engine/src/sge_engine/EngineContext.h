#pragma once

#include <sge_engine/systems/RendererSystem.h>

namespace sge {

class EngineContext : public NonCopyable {
public:
	static EngineContext*	create();
	static void				destroy();

	static EngineContext*	instance() { return s_instance; }
	static RendererSystem*	rendererSystem() { return s_rendererSystem; }

	template<class C>
	void registerComponentType() { _registerComponentType(TypeOf<C>()); }
	void _registerComponentType(const TypeInfo* type);

	const TypeInfo* getComponentType(StrView typeName);

private:
	static EngineContext*	s_instance;
	static RendererSystem*	s_rendererSystem;

	StringMap<const TypeInfo*>	_componentTypeMap;

	EngineContext();
};

}
