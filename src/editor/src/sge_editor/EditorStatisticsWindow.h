#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorStatisticsWindow : public EditorWindow {
public:
	void draw(Scene& scene, RenderRequest& req);
private:
	bool _active = true;

	String _getNumberDesc(u64 n);
};

} // namespace sge