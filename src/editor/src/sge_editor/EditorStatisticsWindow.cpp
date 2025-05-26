#include "EditorStatisticsWindow.h"

namespace sge {

String EditorStatisticsWindow::_getNumberDesc(u64 n) {

	static constexpr u64 K = 1024;
	static constexpr u64 M = 1024 * 1024;
	static constexpr u64 G = 1024 * 1024 * 1024;

	String s;
	if (n >= G) {
		FmtTo(s, "{} G", Math::byteToG(n));
	} else if (n >= M) {
		FmtTo(s, "{} N", Math::byteToM(n));
	} else if (n >= K) {
		FmtTo(s, "{} K", Math::byteToK(n));
	} else {
		FmtTo(s, "{}", n);
	}
	return s;
}

void EditorStatisticsWindow::onDraw(Scene& scene, RenderRequest& req) {
	static int corner = 1; // right top
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (corner != -1)
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= ImGuiWindowFlags_NoMove;
	}

	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	EditorUI::Window win("Statistics", &_active, window_flags);

	const static ImGuiTreeNodeFlags s_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	EditorUI::TreeNode graphics("Graphics", s_flags);
	if (graphics.isOpen()) {
		const static ImGuiTreeNodeFlags s_cflags = s_flags | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

		const auto& frameBufferSize = req.renderContext()->frameBufferSize();
		const auto& statistics		= req.renderContext()->statistics();

		TempString label;
		FmtTo(label, "Screen: {} x {}", static_cast<int>(frameBufferSize.x), static_cast<int>(frameBufferSize.y));
		EditorUI::TreeNode screen(label, s_cflags);
		label.clear();

		FmtTo(label, "DrawCall: {}", statistics.drawCall);
		EditorUI::TreeNode dc(label, s_cflags);
		label.clear();

		String verts = _getNumberDesc(statistics.verts);
		FmtTo(label, "Verts: {}", verts);
		EditorUI::TreeNode vertexes(label, s_cflags);
		label.clear();

		String tris = _getNumberDesc(statistics.tris);
		FmtTo(label, "Tris: {}", tris);
		EditorUI::TreeNode triangles(label, s_cflags);
		label.clear();
	}

#if 0 // TODO: some bug when open 'graphics' node
	EditorUI::TreeNode others("Others", s_flags);
	if (others.isOpen()) {
		const static ImGuiTreeNodeFlags s_cflags = s_flags | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

		TempString label;
		FmtTo(label, "Camera Pos: ({}, {}, {})", Math::roundToInt(req.cameraPos.x), Math::roundToInt(req.cameraPos.y), Math::roundToInt(req.cameraPos.z));
		EditorUI::TreeNode cameraPos(label, s_cflags);
		label.clear();
	}
#endif
}

} // namespace sge