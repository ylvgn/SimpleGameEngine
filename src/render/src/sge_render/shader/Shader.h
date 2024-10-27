#pragma once

#include "ShaderInfo.h"

namespace sge {

class Shader;
class ShaderPass;

#if 0
#pragma mark ========= ShaderStage ============
#endif
class ShaderStage : public NonCopyable {
public:
	ShaderStage() = delete;

	const ShaderStageInfo* info() const { return &_info; }

protected:
	ShaderStage(ShaderPass* pass) noexcept;

	ShaderStageInfo _info;
	ShaderPass*		_pass = nullptr;
}; // ShaderStage

#if 0
#pragma mark ========= ShaderVertexStage ============
#endif
class ShaderVertexStage : public ShaderStage {
	using Base = ShaderStage;
public:
	ShaderVertexStage(ShaderPass* pass) : Base(pass) {}

	static constexpr ShaderStageMask stageMask() { return ShaderStageMask::Vertex; }

}; // ShaderVertexStage

#if 0
#pragma mark ========= ShaderPixelStage ============
#endif
class ShaderPixelStage  : public ShaderStage {
	using Base = ShaderStage;
public:
	ShaderPixelStage(ShaderPass* pass) : Base(pass) {}

	static constexpr ShaderStageMask stageMask() { return ShaderStageMask::Pixel; }

}; // ShaderPixelStage

#if 0
#pragma mark ========= ShaderPass ============
#endif
class ShaderPass : public NonCopyable {
public:
	ShaderPass() = delete;

	using Info = ShaderInfo::Pass;

	virtual ~ShaderPass() = default;

	const Info* info() const { return _info; }

	StrView	shaderFilename() const;

	ShaderVertexStage*	vertexStage() { return _vertexStage; }
	ShaderPixelStage*   pixelStage()  { return _pixelStage; }

	Shader*	shader()	const { return _shader; }
	int		passIndex()	const { return _passIndex; }

friend class Shader;
protected:
	ShaderPass(Shader* shader, int passIndex) noexcept;

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

	void loadFile(StrView filename);

	const String&				filename()		const	{ return _filename; }
	const ShaderInfo*			info()			const	{ return &_info; }
	const ShaderInfo::Pass*		passInfo(int i)	const	{ return _info.passes.inBound(i) ? &_info.passes[i] : nullptr; }
	size_t						passCount()		const	{ return _info.passes.size(); }
	Pass*						findPass(int i)	const	{ return _passes.inBound(i) ? _passes[i].get() : nullptr; }
	Span< UPtr<ShaderPass> >	passes()				{ return _passes; }

protected:
	Shader() = default; // please create from 'Renderer::createShader'

	virtual UPtr<Shader::Pass> onCreatePass(int passIndex) = 0;

	String						_filename;
	ShaderInfo					_info;
	Vector<UPtr<ShaderPass>, 1> _passes;
}; // Shader

} // namespace sge


#define sgeShader_InterfaceFunctions(T) \
	virtual UPtr<Shader::Pass> onCreatePass(int passIndex) final; \
//-----

#define sgeShader_InterfaceFunctions_Impl(T) \
	UPtr<Shader::Pass> Shader_##T::onCreatePass(int passIndex) { return UPtr_make<Shader_##T::Pass>(this, passIndex); } \
//-----