#pragma once

#include "EditorWindow.h"

namespace sge {

class EditorStatisticsWindow : public EditorWindow {
private:
	virtual void onDraw(Scene& scene, RenderRequest& req) final;
	String _getNumberDesc(u64 n);

	bool _active = true;

}; // EditorStatisticsWindow

} // namespace sge