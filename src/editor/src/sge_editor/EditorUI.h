#pragma once

#include "EditorContext.h"

namespace sge {

namespace EditorUI {
	static const char* mixedValueFormat = "--";
	static const char* floatFormat = "%0.3f";
	static bool showMixedValue = false;

	inline bool DragFloat(
		const char* label,
		float* v,
		float v_speed = 0.1f,
		float v_min = std::numeric_limits<float>::lowest(),
		float v_max = std::numeric_limits<float>::max(),
		float v_power = 1.0f)
	{
		return ImGui::DragFloat(label, v, v_speed, v_min, v_max,
			showMixedValue ? mixedValueFormat : floatFormat,
			v_power);
	}

	inline bool DragFloat2(
		const char* label,
		float v[2],
		float v_speed = 0.1f,
		float v_min = std::numeric_limits<float>::lowest(),
		float v_max = std::numeric_limits<float>::max(),
		float v_power = 1.0f)
	{
		return ImGui::DragFloat2(label, v, v_speed, v_min, v_max,
			showMixedValue ? mixedValueFormat : floatFormat,
			v_power);
	}

	inline bool DragFloat3(
		const char* label,
		float v[3],
		float v_speed = 0.1f,
		float v_min = std::numeric_limits<float>::lowest(),
		float v_max = std::numeric_limits<float>::max(),
		float v_power = 1.0f)
	{
		return ImGui::DragFloat3(label, v, v_speed, v_min, v_max,
			showMixedValue ? mixedValueFormat : floatFormat,
			v_power);
	}

	inline bool DragFloat4(
		const char* label,
		float v[4],
		float v_speed = 0.1f,
		float v_min = std::numeric_limits<float>::lowest(),
		float v_max = std::numeric_limits<float>::max(),
		float v_power = 1.0f)
	{
		return ImGui::DragFloat4(label, v, v_speed, v_min, v_max,
			showMixedValue ? mixedValueFormat : floatFormat,
			v_power);
	}

	inline float InputFloat(const char* label, float* v) {
		return ImGui::InputFloat(label, v, 0, 0,
			showMixedValue ? mixedValueFormat : floatFormat,
			ImGuiInputTextFlags_EnterReturnsTrue);
	}

	inline float InputFloat2(const char* label, float v[2]) {
		return ImGui::InputFloat2(label, v,
			showMixedValue ? mixedValueFormat : floatFormat,
			ImGuiInputTextFlags_EnterReturnsTrue);
	}

	inline float InputFloat3(const char* label, float v[3]) {
		return ImGui::InputFloat3(label, v,
			showMixedValue ? mixedValueFormat : floatFormat,
			ImGuiInputTextFlags_EnterReturnsTrue);
	}

	inline float InputFloat4(const char* label, float v[4]) {
		return ImGui::InputFloat4(label, v,
			showMixedValue ? mixedValueFormat : floatFormat,
			ImGuiInputTextFlags_EnterReturnsTrue);
	}

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

		~DragDropTarget() { if(_isOpen) ImGui::EndDragDropTarget(); }

		bool isOpen() const { return _isOpen; }
		const ImGuiPayload* acceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0) {
			if (!_isOpen) return nullptr;
			return ImGui::AcceptDragDropPayload(type, flags);
		}
	private:
		bool _isOpen;
	};

	class CollapsingHeader : public NonCopyable {
	public:
		CollapsingHeader(const char* label) {
			_isOpen = ImGui::CollapsingHeader(label, &_visiable);
		}

		bool isVisible() const { return _visiable; }
		bool isOpen() const { return _isOpen; }
	private:
		bool _visiable = true;
		bool _isOpen;
	};

	class PushID : public NonCopyable {
	public:
		PushID(const void* id) { ImGui::PushID(id); }
		~PushID() { ImGui::PopID(); }
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