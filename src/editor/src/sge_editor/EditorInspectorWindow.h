#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorInspectorWindow : public EditorWindow {
public:
	void draw(Scene& scene, RenderRequest& req);
private:
	bool _active = true;

	using ObjectList = Vector<Object*, 32>;
	Map<const TypeInfo*, ObjectList>	_tempTypeMap;
	Vector<ObjectList*>					_tempTypeList;

	void _addTempList(Component* c);
	void drawComponent(RenderRequest& req, ObjectList& list);

};

}