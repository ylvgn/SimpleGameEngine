#pragma once

namespace sge {

class CSystemBase;
class CRendererSystem;

class EngineContext : public NonCopyable {
public:
	static EngineContext*	create();
	static void				destroy();

	static EngineContext*	instance()			{ return s_instance; }

	template<class C>
	void registerComponentType() { _registerComponentType(TypeOf<C>()); }
	void _registerComponentType(const TypeInfo* type);

	template<class C>
	void registerSystem() { static_assert(std::is_base_of<CSystemBase, C>::value); _registerSystem(new C); }
	void _registerSystem(CSystemBase* sys);

	const TypeInfo* findTypeByName(StrView typeName);

private:
	static EngineContext*	s_instance;

	StringMap<const TypeInfo*>	_componentTypeMap;
	Vector< UPtr<CSystemBase> >	_systems;
	
	EngineContext();
};

inline const TypeInfo* EngineContext::findTypeByName(StrView name) {
	auto it = _componentTypeMap.find(TempString(name).c_str());
	if (it == _componentTypeMap.end()) return nullptr;
	return it->second;
}

}
