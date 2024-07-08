#pragma once

#include "../NeHeOGL 009/NeHeOGL_Lesson009_NativeUIWindow.h"

namespace sge {
/*
	* Loading And Moving Through A 3D World
	* FYI: https://nehe.gamedev.net/tutorial/loading_and_moving_through_a_3d_world/22003/
*/

class NeHeOGL_Lesson010 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson010;
public:
	typedef struct tagVERTEX {
		Vec3f pos;
		Vec2f uv;
	} VERTEX;

	typedef struct tagTRIANGLE {
		Vector<VERTEX, 3> vertex;
	} TRIANGLE;

	typedef struct tagSECTOR
	{
		Vector<TRIANGLE> triangles;

		void clear() {
			triangles.clear();
		}
	} SECTOR;

	class WorldLoader : public NonCopyable {
	public:
		static void s_readFile(SECTOR& outInfo, StrView filename);
		static void s_readMem(SECTOR& outInfo, ByteSpan bytes, StrView filename);
	private:
		void _readMem(SECTOR& outInfo, ByteSpan bytes, StrView filename);
		void _nextLine();
		void _nextToken();
		void _parseLine();

		void _error(StrView msg) { throw SGE_ERROR("{}: {}", _lineNumber, msg); }

		void _parseLine_NUMPOLLIES();
		void _parseLine_tagTRIANGLE();
		void _internal_parseLine_tagTRIANGLE(TRIANGLE& out, int index);

		SECTOR* _outInfo = nullptr;
		StrView _filename;

		StrView _source;
		StrView _sourceRemain;

		StrView _currentLine;
		StrView _lineRemain;

		String  _token;

		int		_lineNumber = 0;

		int		_tagSECTOR_index = 0;
		int		_tagTRIANGLE_index = 0;
	};

protected:
	virtual void onInitedGL() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override;
	virtual void onDraw() override;
private:
	const float kPIOver180 = Math::PI<float>() / 180.f;

	void _example1();

	SECTOR _sector;

	NeHeOGL_Texture2D _tex;

	float _walkBiasAngle = 0.f;

	Vec3f _camerMovePos		{ 0, 0.25f, 0.5f };
	Vec2f _camerOrbitAngle	{ 10,0 };
};

}