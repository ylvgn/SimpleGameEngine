#pragma once

#include "EntitySelection.h"
#include "EditorPropertyDrawer.h"

namespace sge {

class EditorContext : public NonCopyable {
public:
	EditorContext() noexcept;

	static EditorContext* instance() { return s_instance; }

	static EditorContext* create();
	static void destroy();

	void registerPropertyDrawer(const TypeInfo* type, EditorPropertyDrawer* drawer);
	EditorPropertyDrawer* getPropertyDrawer(const TypeInfo* type);

	EntitySelection	entitySelection;

private:
	static EditorContext* s_instance;

	Map<const TypeInfo*, EditorPropertyDrawer_struct>	_defaultStructPropertyDrawerMap;
	Map<const TypeInfo*, EditorPropertyDrawer*>			_propertyDrawerMap;
};

} // namespace sge