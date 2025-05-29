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
	const ShaderStageInfo*	info()		const { return &_info; }
	const ShaderInfo::Pass* passInfo()	const;

	virtual ShaderStageMask stageMask() { return ShaderStageMask::None; }

protected:
	ShaderStage(ShaderPass* pass) noexcept;

	ShaderStageInfo _info;
	ShaderPass*		_pass = nullptr;
}; // ShaderStage


#if 0
#pragma mark ========= ShaderVertexStage ============
#endif
class ShaderVertexStage : public ShaderStage {
	using This = ShaderVertexStage;
	using Base = ShaderStage;
public:
	static constexpr ShaderStageMask s_stageMask() { return ShaderStageMask::Vertex; }

	ShaderVertexStage(ShaderPass* pass) noexcept : Base(pass) {}
	virtual ShaderStageMask stageMask() override { return This::s_stageMask(); }
}; // ShaderVertexStage


#if 0
#pragma mark ========= ShaderPixelStage ============
#endif
class ShaderPixelStage  : public ShaderStage {
	using This = ShaderPixelStage;
	using Base = ShaderStage;
public:
	static constexpr ShaderStageMask s_stageMask() { return ShaderStageMask::Pixel; }

	ShaderPixelStage(ShaderPass* pass) noexcept : Base(pass) {}
	virtual ShaderStageMask stageMask() override { return This::s_stageMask(); }
}; // ShaderPixelStage


#if 0
#pragma mark ========= ShaderComputeStage ============
#endif
class ShaderComputeStage : public ShaderStage {
	using This = ShaderComputeStage;
	using Base = ShaderStage;
public:
	static constexpr ShaderStageMask s_stageMask() { return ShaderStageMask::Compute; }

	ShaderComputeStage(ShaderPass* pass) noexcept : Base(pass) {}
	virtual ShaderStageMask stageMask() override { return This::s_stageMask(); }
}; // ShaderComputeStage


#if 0
#pragma mark ========= ShaderPass ============
#endif
class ShaderPass : public NonCopyable {
public:
	using Info = ShaderInfo::Pass;

	virtual ~ShaderPass() = default;

	const Info* info() const { return _info; }

	StrView	shaderFilename() const;

	ShaderVertexStage*	vertexStage()	const { return _vertexStage; }
	ShaderPixelStage*   pixelStage()	const { return _pixelStage; }
	ShaderComputeStage*	computeStage()	const { return _computeStage; }

	Shader*	shader()	const { return _shader; }
	int		passIndex()	const { return _passIndex; }

	bool	        isCompute()  const { return BitUtil::has(_stageMasks, ShaderStageMask::Compute); }
	ShaderStageMask stageMasks() const { return _stageMasks; }

	bool hasVS() const { return BitUtil::hasAny(_stageMasks, ShaderStageMask::Vertex); }
	bool hasPS() const { return BitUtil::hasAny(_stageMasks, ShaderStageMask::Pixel); }
	bool hasCS() const { return BitUtil::hasAny(_stageMasks, ShaderStageMask::Compute); }

friend class Shader;
protected:
	ShaderPass(Shader* shader, int passIndex) noexcept;

	virtual void onInit() = 0;

	const Info*			_info			= nullptr;
	Shader*				_shader			= nullptr;

	ShaderVertexStage*	_vertexStage	= nullptr;
	ShaderPixelStage*   _pixelStage		= nullptr;
	ShaderComputeStage* _computeStage	= nullptr;

	ShaderStageMask		_stageMasks     = ShaderStageMask::None;
	int					_passIndex		= -1;
}; // ShaderPass

SGE_INLINE // TODO: may write to a new cpp file
const ShaderInfo::Pass* ShaderStage::passInfo() const {
	return _pass->info();
}


#if 0
#pragma mark ========= Shader ============
#endif
class Shader : public Object {
public:
	using Pass			= ShaderPass;
	using Stage			= ShaderStage;
	using VertexStage	= ShaderVertexStage;
	using PixelStage	= ShaderPixelStage;
	using ComputeStage  = ShaderComputeStage;

	virtual ~Shader();

	void loadFile(StrView filename);

	const String&				filename()		const	{ return _filename; }
	const ShaderInfo*			info()			const	{ return &_info; }
	const ShaderInfo::Pass*		passInfo(int i)	const	{ return _info.passes.inBound(i) ? &_info.passes[i] : nullptr; }
	size_t						passCount()		const	{ return _info.passes.size(); }
	Pass*						findPass(int i)	const	{ return _passes.inBound(i) ? _passes[i].get() : nullptr; }
	Span< UPtr<ShaderPass> >	passes()				{ return _passes; }

protected:
	Shader() noexcept = default; // please create from 'Renderer::createShader'

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