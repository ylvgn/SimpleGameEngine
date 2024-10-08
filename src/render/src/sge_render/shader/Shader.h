#pragma once

#include "ShaderInfo.h"

namespace sge {

class Shader;

struct ShaderStage : public NonCopyable {
	const ShaderStageInfo* info() const { return &_info; }
protected:
	ShaderStageInfo _info;
};

struct ShaderVertexStage : public ShaderStage {
	static constexpr ShaderStageMask stageMask() { return ShaderStageMask::Vertex; }
};
struct ShaderPixelStage  : public ShaderStage {
	static constexpr ShaderStageMask stageMask() { return ShaderStageMask::Pixel; }
};

struct ShaderPass : public NonCopyable {
	ShaderPass(Shader* shader, ShaderInfo::Pass& info);

	virtual ~ShaderPass() noexcept = default;

	ShaderVertexStage* vertexStage()		{ return _vertexStage; }
	ShaderPixelStage*   pixelStage()		{ return _pixelStage; }
	const ShaderInfo::Pass*	info() const	{ return _info; }

protected:
	Shader*				_shader			= nullptr;
	ShaderInfo::Pass*   _info			= nullptr;
	ShaderVertexStage*	_vertexStage	= nullptr;
	ShaderPixelStage*   _pixelStage		= nullptr;
};

class Shader : public RefCountBase {
public:
	using Pass			= ShaderPass;
	using Stage			= ShaderStage;
	using VertexStage	= ShaderVertexStage;
	using PixelStage	= ShaderPixelStage;

	Shader(StrView filename);
	virtual ~Shader();

	const String& filename() const { return _filename; }

	const ShaderInfo* info() const { return &_info; }

	Span< UPtr<ShaderPass> > passes() { return _passes; }

protected:
	String	_filename;
	ShaderInfo	_info;
	Vector<UPtr<ShaderPass>, 1> _passes;
};

}