#pragma once
namespace sge {

class Lexer;

enum class ShaderDataType {
	None,
	Int,
	Float,
	Float2,
	Float3,
	Float4,
	Color4f,
};

struct ShaderInfo {

	struct Property {
		ShaderDataType type = ShaderDataType::None;
		String name;
		String value;
	};

	struct Pass {
		String name;
		String vsFunc;
		String psFunc;
	};

	Vector<Property> properties;
	Vector<Pass> passes;

	void clean() {
		properties.clear();
		passes.clear();
	}
};

class ShaderParser : public Lexer {
public:
	ShaderParser() = default;
	static void loadFile(ShaderInfo& outputInfo, StrView filename);
	static void loadMem(ShaderInfo& outputInfo, Span<const u8> src);
private:
	void _loadMem(ShaderInfo& outputInfo, Span<const u8> src);

	ShaderInfo* _outInfo = nullptr;

	int _lineNumber = 0;
	void _error(StrView msg) {
		throw SGE_ERROR("line:{} :{}", _lineNumber, msg);
	}

}; // ShaderParser

} // namespace
