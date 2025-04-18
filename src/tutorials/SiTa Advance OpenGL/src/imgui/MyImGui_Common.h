#pragma once

#include "imgui.h"

#include "../native_ui/my_nativeui.h"
#include "../render/my_render.h"

inline void operator += (ImVec2& a, const ImVec2& b) { a.x += b.x; a.y += b.y; }
inline void operator -= (ImVec2& a, const ImVec2& b) { a.x -= b.x; a.y -= b.y; }
inline void operator *= (ImVec2& a, const ImVec2& b) { a.x *= b.x; a.y *= b.y; }
inline void operator /= (ImVec2& a, const ImVec2& b) { a.x /= b.x; a.y /= b.y; }

inline ImVec2 operator + (const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator - (const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2 operator * (const ImVec2& a, const ImVec2& b) { return ImVec2(a.x * b.x, a.y * b.y); }
inline ImVec2 operator / (const ImVec2& a, const ImVec2& b) { return ImVec2(a.x / b.x, a.y / b.y); }

inline ImVec2 operator + (const ImVec2& a, float b) { return ImVec2(a.x + b, a.y + b); }
inline ImVec2 operator - (const ImVec2& a, float b) { return ImVec2(a.x - b, a.y - b); }
inline ImVec2 operator * (const ImVec2& a, float b) { return ImVec2(a.x * b, a.y * b); }
inline ImVec2 operator / (const ImVec2& a, float b) { return ImVec2(a.x / b, a.y / b); }

namespace sge {

template<> inline
ByteSpan ByteSpan_make(Span<ImDrawVert> src) {
	using DST = typename ByteSpan::element_type;
	size_t sizeInBytes = src.size() * sizeof(ImDrawVert);
	return Span<DST>(reinterpret_cast<DST*>(src.data()), sizeInBytes / sizeof(DST));
}

inline
ByteSpan toSpan(const ImVector<ImDrawVert>& src) {
	using SRC = typename ImDrawVert;
	Span<const SRC> s(src.begin(), src.end());
	return ByteSpan_make(s);
}

inline
Span<const MyEditMesh::IndexType> toSpan(const ImVector<ImDrawIdx>& src) {
	using SRC = typename ImDrawIdx;
	using DST = const MyEditMesh::IndexType;
	Span<const SRC> s(src.begin(), src.end());
	return spanCast<DST>(s);
}

}