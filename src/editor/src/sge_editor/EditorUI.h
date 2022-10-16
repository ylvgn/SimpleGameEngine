#pragma once

#include "EditorContext.h"

namespace sge {

namespace EditorUI {

	class Window : public NonCopyable {
	public:
		Window(StrView name, bool* p_open = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar) {
			ImGui::Begin(TempString(name).c_str(), p_open, flags);
		}

		~Window() { ImGui::End(); }
	};

	class TreeNode : public NonCopyable {
	public:
		TreeNode(StrView label, ImGuiTreeNodeFlags flags = 0) {
			_isOpen = ImGui::TreeNodeEx(TempString(label).c_str(), flags);
			_isLeaf = (flags & ImGuiTreeNodeFlags_Leaf) > 0;
		}

		~TreeNode() { if (_isOpen && !_isLeaf) ImGui::TreePop(); }

		bool isOpen() const { return _isOpen; }
		bool isLeaf() const { return _isLeaf; }
	private:
		bool _isOpen;
		bool _isLeaf;
	};

	class DragDropSource : public NonCopyable {
	public:
		DragDropSource(ImGuiDragDropFlags flags = ImGuiDragDropFlags_None) {
			_isOpen = !ImGui::GetDragDropPayload() && ImGui::BeginDragDropSource(flags);
		}

		~DragDropSource() { if (_isOpen) ImGui::EndDragDropSource(); }

		bool isOpen() const { return _isOpen; }
	private:
		bool _isOpen;
	};

	class DragDropTarget : public NonCopyable {
	public:
		DragDropTarget() {
			_isOpen = ImGui::BeginDragDropTarget();
		}

		~DragDropTarget() { if (_isOpen) ImGui::EndDragDropTarget(); }

		bool isOpen() const { return _isOpen; }
	private:
		bool _isOpen;
	};

	inline bool IsItemToggledOpen() { return ImGui::IsItemToggledOpen(); }
	inline bool IsItemClicked()		{ return ImGui::IsItemClicked(); }
	inline bool IsItemActive()		{ return ImGui::IsItemActive(); }
	inline bool IsItemHovered()		{ return ImGui::IsItemHovered(); }

	inline bool IsKeyShift()		{ return ImGui::GetIO().KeyShift; }
	inline bool IsKeyAlt()			{ return ImGui::GetIO().KeyAlt; }
	inline bool IsKeyCtrl()			{ return ImGui::GetIO().KeyCtrl; }

} // EditorUI

} // sge