#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorInspectorWindow : public EditorWindow {
private:
	using ObjectList = Vector<Object*, 32>;

	virtual void onDraw(Scene& scene, RenderRequest& req) final;

	void _addTempList(Component* c);
	void _drawComponent(RenderRequest& req, ObjectList& list);

	bool _active = true;

	Map<const TypeInfo*, ObjectList>	_tempTypeMap;
	Vector<ObjectList*>					_tempTypeList;
}; // EditorInspectorWindow

} // namespace sge