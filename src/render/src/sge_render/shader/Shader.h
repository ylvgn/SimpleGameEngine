#pragma once

#include "ShaderInfo.h"

namespace sge {

class Shader;

#if 0
#pragma mark ========= ShaderStage ============
#endif
struct ShaderStage : public NonCopyable {
	const ShaderStageInfo* info() const { return &_info; }
protected:
	ShaderStageInfo _info;
}; // ShaderStage

#if 0
#pragma mark ========= ShaderVertexStage ============
#endif
struct ShaderVertexStage : public ShaderStage {
	static constexpr ShaderStageMask stageMask() { return ShaderStageMask::Vertex; }
}; // ShaderVertexStage

#if 0
#pragma mark ========= ShaderPixelStage ============
#endif
struct ShaderPixelStage  : public ShaderStage {
	static constexpr ShaderStageMask stageMask() { return ShaderStageMask::Pixel; }
}; // ShaderPixelStage

#if 0
#pragma mark ========= ShaderPass ============
#endif
struct ShaderPass : public NonCopyable {
	using Info = ShaderInfo::Pass;

	ShaderPass(Shader* shader, int passIndex) noexcept;

	virtual ~ShaderPass() noexcept = default;

	ShaderVertexStage*	vertexStage()		{ return _vertexStage; }
	ShaderPixelStage*   pixelStage()		{ return _pixelStage; }

	Shader* shader()			{ return _shader; }
	int		passIndex()			{ return _passIndex; }

	const Info* info()				const	{ return _info; }
	StrView		shaderFilename()	const;

friend class Shader;
protected:
	virtual void onInit() = 0;

	const Info*			_info			= nullptr;
	ShaderVertexStage*	_vertexStage	= nullptr;
	ShaderPixelStage*   _pixelStage		= nullptr;
	Shader*				_shader			= nullptr;
	int					_passIndex		= -1;
}; // ShaderPass

#if 0
#pragma mark ========= Shader ============
#endif
class Shader : public RefCountBase {
public:
	using Pass			= ShaderPass;
	using Stage			= ShaderStage;
	using VertexStage	= ShaderVertexStage;
	using PixelStage	= ShaderPixelStage;

	virtual ~Shader();

	void _internal_init();

	const String&				filename()		const	{ return _filename; }
	const ShaderInfo*			info()			const	{ return &_info; }
	const ShaderInfo::Pass*		passInfo(int i)	const	{ return _info.passes.inBound(i) ? &_info.passes[i] : nullptr; }
	size_t						passCount()		const	{ return _info.passes.size(); }

	Span< UPtr<ShaderPass> >	passes()				{ return _passes; }
	Pass*						findPass(int i)			{ return _passes.inBound(i) ? _passes[i].get() : nullptr; }

protected:
	Shader(StrView filename);  // please create from 'Renderer::createShader'

	virtual UPtr<ShaderPass> onCreateShaderPass	(Shader* shader, int passIndex) = 0;

	String						_filename;
	ShaderInfo					_info;
	Vector<UPtr<ShaderPass>, 1> _passes;
}; // Shader

} // namespace sge