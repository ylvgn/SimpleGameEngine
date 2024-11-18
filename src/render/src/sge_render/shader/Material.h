#pragma once

#include "Shader.h"
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/textures/Texture.h>

namespace sge {

class RenderContext;
class Material;
class MaterialPass;

class RenderCommand_DrawCall;

#if 0
#pragma mark ========= MaterialPass_Stage ============
#endif
class MaterialPass_Stage : public NonCopyable {
	using Pass = MaterialPass;
public:
	MaterialPass_Stage(MaterialPass* pass, ShaderStage* shaderStage);
	virtual ~MaterialPass_Stage() = default;

friend class MaterialPass;
protected:

	// const buffer
	struct ConstBuffer {
		using DataType	= ShaderStageInfo::DataType;
		using Info		= ShaderStageInfo::ConstBuffer;
		using VarInfo	= ShaderStageInfo::Variable;

		Vector<u8>				cpuBuffer;
		SPtr<RenderGpuBuffer>	gpuBuffer;

		template<class V>
		void setParam(const Info& cbInfo, StrView name, const V& value) {
			auto* varInfo = cbInfo.findVariable(name);
			if (!varInfo) return;
			_setParam(varInfo, value);
		}

		void create(const Info& info);

		void uploadToGpu();

		const Info* info() const { return _info; }
	private:
		void errorType();

		void _setParam(const VarInfo* varInfo, const i32&     value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const u32&     value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const float&   value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple2i& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple3i& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple4i& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple2f& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple3f& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple4f& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Mat4f&   value) { _setParamCheckType(varInfo, value); }

		template<class V>
		void _setParamCheckType(const VarInfo* varInfo, const V& value) {
			if (varInfo->dataType != RenderDataTypeUtil::get<V>()) {
				errorType();
				return;
			}
			_setValueAs(varInfo, value);
		}

		template<class V>
		void _setValueAs(const VarInfo* varInfo, const V& value) {
			auto end = varInfo->offset + sizeof(value);
			if (end > cpuBuffer.size())
				throw SGE_ERROR("ConstBuffer setParam out of range");
			auto* dst = cpuBuffer.data() + varInfo->offset;
			*reinterpret_cast<V*>(dst) = value;
			_gpuDirty = true;
		}

		const Info*		_info = nullptr;
		bool			_gpuDirty = false;
	};

	template<class V>
	void _setParam(StrView name, const V& v) {
		if (!_shaderStage) return;
		size_t i = 0;
		for (auto& cb : _constBuffers) {
			cb.setParam(info()->constBuffers[i], name, v);
			i++;
		}
	}

	// texture
	struct TexParam {
		using DataType	= ShaderStageInfo::DataType;
		using Info		= ShaderStageInfo::Texture;

		void create(const Info& info) { _info = &info; }

		Texture*	getUpdatedTexture();

		StrView		name() const		{ return _info->name; }
		int			bindPoint() const	{ return _info->bindPoint; }
		DataType	dataType() const	{ return _info->dataType; }

		template<class TEX>
		void setTexParam(TEX* tex) {
			if (_info->dataType != RenderDataTypeUtil::get<TEX>()) {
				_tex.reset(nullptr);
				throw SGE_ERROR("invalid texture type");
			}
			_tex = tex;
		}

	protected:
		SPtr<Texture>	_tex;
		const Info*		_info = nullptr;
	};

	template<class V>
	void _setTexParam(StrView name, V* texture) {
		if (!texture) { SGE_ASSERT(false); return; }

		for (auto& p : _texParams) {
			if (0 == p.name().compare(name)) {
				p.setTexParam(texture);
				break;
			}
		}
	}

	Vector<ConstBuffer, 4>	_constBuffers;
	Vector<TexParam, 4>		_texParams;
	ShaderStage*			_shaderStage	= nullptr;
	Pass*					_pass			= nullptr;

public:
	const ShaderStageInfo*	info() const { return _shaderStage->info(); }

	Span<ConstBuffer>	constBuffers()	 { return _constBuffers; }
	Span<TexParam>		texParams()		 { return _texParams; }

}; // MaterialPass_Stage


#if 0
#pragma mark ========= MaterialPass_VertexStage ============
#endif
class MaterialPass_VertexStage : public MaterialPass_Stage {
	using Base = MaterialPass_Stage;
public:
	MaterialPass_VertexStage(MaterialPass* pass, ShaderVertexStage* shaderStage);
}; // MaterialPass_VertexStage


#if 0
#pragma mark ========= MaterialPass_PixelStage ============
#endif
class MaterialPass_PixelStage : public MaterialPass_Stage {
	using Base = MaterialPass_Stage;
public:
	MaterialPass_PixelStage(MaterialPass* pass, ShaderPixelStage* shaderStage);
}; // MaterialPass_PixelStage


#if 0
#pragma mark ========= MaterialPass ============
#endif
class MaterialPass : public NonCopyable {
public:
	MaterialPass() = delete;
	virtual ~MaterialPass() = default;

	using Pass			= MaterialPass;
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

	using ShaderPassInfo = ShaderInfo::Pass;

	const ShaderPassInfo*	info()			const { return _shaderPass->info(); }
	const RenderState&		renderState()	const { return info()->renderState; }

	void bind(RenderContext* ctx, RenderCommand_DrawCall& drawCall) { onBind(ctx, drawCall); }

friend class Material;
protected:
	MaterialPass(Material* material, ShaderPass* shaderPass) noexcept;

	virtual void onBind(RenderContext* ctx, RenderCommand_DrawCall& drawCall) = 0;

	template<class V>
	void _setParam(StrView name, const V& v) {
		if (_vertexStage) _vertexStage->_setParam(name, v);
		 if (_pixelStage)  _pixelStage->_setParam(name, v);
	}

	template<class V>
	void _setTexParam(StrView name, const V& v) {
		if (_vertexStage) _vertexStage->_setTexParam(name, v);
		 if (_pixelStage)  _pixelStage->_setTexParam(name, v);
	}

	Material*	 _material		= nullptr;
	ShaderPass*  _shaderPass	= nullptr;
	VertexStage* _vertexStage	= nullptr;
	PixelStage*  _pixelStage	= nullptr;
}; // MaterialPass


#if 0
#pragma mark ========= Material ============
#endif
class Material : public RefCountBase {
public:
	virtual ~Material() = default;

	using Pass			= MaterialPass;
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

	void setParam(StrView name, Texture2D*     v) { _setTexParam(name, v); }
	void setParam(StrView name, const i32&     v) { _setParam(name, v); }
	void setParam(StrView name, const u32&     v) { _setParam(name, v); }
	void setParam(StrView name, const float&   v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple2i& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple3i& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple4i& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple2f& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple3f& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple4f& v) { _setParam(name, v); }
	void setParam(StrView name, const Mat4f&   v) { _setParam(name, v); }

	void setShader(Shader* shader);
	Span< UPtr<Pass> >	passes() { return _passes; }

	Pass* getPass(size_t index) const {
		if (index >= _passes.size()) {
			SGE_ASSERT(false);
			return nullptr;
		}
		return _passes[index].get();
	}

protected:
	template<class V>
	void _setParam(StrView name, const V& v) {
		for (auto& pass : _passes) {
			if (pass) pass->_setParam(name, v);
		}
	}

	template<class V>
	void _setTexParam(StrView name, const V& v) {
		for (auto& pass : _passes) {
			if (pass) pass->_setTexParam(name, v);
		}
	}

	virtual void onSetShader() {}
	virtual UPtr<Pass> onCreatePass(ShaderPass* shaderPass) = 0;

	SPtr<Shader>			_shader;
	Vector<UPtr<Pass>, 2>	_passes;

}; // Material

} // namespace sge


#define sgeMaterial_InterfaceFunctions(T) \
	virtual UPtr<Material::Pass> onCreatePass(Shader::Pass* shaderPass) final;
//-----

#define sgeMaterial_InterfaceFunctions_Impl(T) \
	UPtr<Material::Pass> Material_##T::onCreatePass(Shader::Pass* shaderPass) { return UPtr_make<Material_##T::Pass>(this, shaderPass); } \
//-----