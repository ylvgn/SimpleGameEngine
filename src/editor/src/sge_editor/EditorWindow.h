#pragma once

#include "EditorUI.h"

namespace sge {

class Scene;
class RenderRequest;

class EditorWindow : public NonCopyable {
public:
	void draw(Scene& scene, RenderRequest& req) { onDraw(scene, req); }
protected:
	virtual void onDraw(Scene& scene, RenderRequest& req) {};
}; // EditorWindow

} // namespace sge