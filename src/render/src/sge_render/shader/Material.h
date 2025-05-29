#pragma once

#include "Shader.h"
#include <sge_render/buffer/RenderGpuStorageBuffer.h>
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

		void _setParam(const VarInfo* varInfo, const     i32& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const     u32& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const     f32& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple2i& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple3i& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple4i& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple2f& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple3f& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Tuple4f& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const   Mat4f& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Color4b& value) { _setParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const Color4f& value) { _setParamCheckType(varInfo, value); }

		void _setParam(const VarInfo* varInfo, const   RenderInt32x2Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const   RenderInt32x3Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const   RenderInt32x4Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const RenderFloat32x2Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const RenderFloat32x3Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const RenderFloat32x4Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const RenderFloat64x2Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const RenderFloat64x3Array& value) { _setArrayParamCheckType(varInfo, value); }
		void _setParam(const VarInfo* varInfo, const RenderFloat64x4Array& value) { _setArrayParamCheckType(varInfo, value); }

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

		template<class V>
		void _setArrayParamCheckType(const VarInfo* varInfo, const V& value) {
			if (varInfo->dataType != RenderDataTypeUtil::get<V>()) {
				errorType();
				return;
			}
			_setArrayValueAs(varInfo, value);
		}

		template<class V>
		void _setArrayValueAs(const VarInfo* varInfo, const V& value) {
			if (value.size() == 0)
				return;

			u8* dst = cpuBuffer.data() + varInfo->offset;
			auto end = varInfo->offset + value.sizeInBytes();
			if (end > cpuBuffer.size())
				throw SGE_ERROR("ConstBuffer setParam '{}' out of range", varInfo->name);

			if (varInfo->dataSize % V::s_PackingSizeInBytes() == 0)
			{
				memcpy(dst, value.data(), value.sizeInBytes());
			}
			else
			{
				using ElementType = V::ElementType;

				int remainingSizeInBytes = V::s_PackingSizeInBytes();
				for (int i = 0; i < value.size(); ++i) {
					remainingSizeInBytes -= V::s_ElementSizeInBytes();
					memcpy(dst, &value[i], V::s_ElementSizeInBytes());
					
					size_t byteOffset = static_cast<size_t>(ptrdiff_t(dst - cpuBuffer.data()));
//					SGE_DUMP_VAR(i, byteOffset, *reinterpret_cast<ElementType*>(dst));
					if (byteOffset + V::s_ElementSizeInBytes() > cpuBuffer.size()) {
						throw SGE_ERROR("ConstBuffer setParam '{}[{}]' out of range", varInfo->name, i);
					}

					if (remainingSizeInBytes < V::s_ElementSizeInBytes()) {
						dst += V::s_PackingSizeInBytes();
						remainingSizeInBytes = V::s_PackingSizeInBytes();
					} else {
						dst += V::s_ElementSizeInBytes();
					}
				}
			}
			_gpuDirty = true;
		}

		const Info*		_info	  = nullptr;
		bool			_gpuDirty = false;
	}; // ConstBuffer

//----------
	template<class V>
	void _setParam(StrView name, const V& v) {
		if (!_shaderStage) return;
		size_t i = 0;
		for (auto& cb : _constBuffers) {
			cb.setParam(info()->constBuffers[i], name, v);
			i++;
		}
	}

//----------
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

//----------
	struct StorageBufferParam {
		using DataType	= ShaderStageInfo::DataType;
		using Info		= ShaderStageInfo::StorageBuffer;

		void create(const Info& info) { _info = &info; }

		StrView					name()				const	{ return _info->name; }
		int						bindPoint()			const	{ return _info->bindPoint; }
		int						bindCount()			const	{ return _info->bindCount; }
		bool					rawUAV()			const	{ return _info->rawUAV; }
		RenderGpuStorageBuffer* storageGpuBuffer()			{ return _storageGpuBuffer; }

		template<class STORAGE>
		void setStorageBufferParam(STORAGE* storageGpuBuffer) {
			_storageGpuBuffer = storageGpuBuffer;
		}
	protected:
		SPtr<RenderGpuStorageBuffer> _storageGpuBuffer;
		const Info* _info = nullptr;
	};

	template<class V>
	void _setStorageBufferParam(StrView name, V* storageGpuBuffer) {
		if (!storageGpuBuffer) { SGE_ASSERT(false); return; }

		for (auto& p : _storageBufParams) {
			if (0 == p.name().compare(name)) {
				p.setStorageBufferParam(storageGpuBuffer);
				break;
			}
		}
	}

	Vector<ConstBuffer, 4>			_constBuffers;
	Vector<TexParam, 4>				_texParams;
	Vector<StorageBufferParam,  4>	_storageBufParams;

	ShaderStage*	_shaderStage	= nullptr;
	Pass*			_pass			= nullptr;

public:
	const ShaderStageInfo*		    info()  const	{ return _shaderStage->info(); }
	const ShaderInfo::Pass*		passInfo()  const	{ return _shaderStage->passInfo(); }
		  ShaderStageMask		stageMask()	const	{ return _shaderStage->stageMask(); }

	bool isVS() const { return BitUtil::has(stageMask(), ShaderStageMask::Vertex); }
	bool isPS() const { return BitUtil::has(stageMask(), ShaderStageMask::Pixel); }
	bool isCS() const { return BitUtil::has(stageMask(), ShaderStageMask::Compute); }

	Span<ConstBuffer>			constBuffers()		{ return _constBuffers; }
	Span<TexParam>				texParams()			{ return _texParams; }
	Span<StorageBufferParam>	storageBufParams()	{ return _storageBufParams; }
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
#pragma mark ========= MaterialPass_ComputeStage ============
#endif
class MaterialPass_ComputeStage : public MaterialPass_Stage {
	using Base = MaterialPass_Stage;
public:
	MaterialPass_ComputeStage(MaterialPass* pass, ShaderComputeStage* shaderStage);
}; // MaterialPass_ComputeStage


#if 0
#pragma mark ========= MaterialPass ============
#endif
class MaterialPass : public NonCopyable {
public:
	MaterialPass() = delete;
	virtual ~MaterialPass() = default;

	using ShaderPassInfo = ShaderInfo::Pass;
	using Pass			 = MaterialPass;
	using Stage			 = MaterialPass_Stage;
	using VertexStage	 = MaterialPass_VertexStage;
	using PixelStage	 = MaterialPass_PixelStage;
	using ComputeStage   = MaterialPass_ComputeStage;

	const ShaderPassInfo*	info()			const { return _shaderPass->info(); }
	const RenderState&		renderState()	const { return info()->renderState; }
	ShaderStageMask			stageMasks()	const { return _shaderPass->stageMasks(); }

	bool hasVS() const { return _shaderPass->hasVS(); }
	bool hasPS() const { return _shaderPass->hasPS(); }
	bool hasCS() const { return _shaderPass->hasCS(); }

	void bind  (RenderContext& ctx, RenderCommand_DrawCall& drawCall) { onBind(&ctx, drawCall); }
	void unbind(RenderContext& ctx, RenderCommand_DrawCall& drawCall) { onUnbind(&ctx, drawCall); }

	struct ScopedBind {
		ScopedBind(MaterialPass& pass_, RenderContext& ctx_, RenderCommand_DrawCall& call_)
			: pass(pass_)
			, ctx(ctx_)
			, call(call_)
		{
			pass.bind(ctx, call);
		}
		~ScopedBind() {
			pass.unbind(ctx, call);
		}
		MaterialPass& pass;
		RenderContext& ctx;
		RenderCommand_DrawCall& call;
	}; // ScopedBind

	SGE_NODISCARD ScopedBind scopedBind(RenderContext& ctx, RenderCommand_DrawCall& call) { return ScopedBind(*this, ctx, call); }

friend class Material;
protected:
	MaterialPass(Material* material, ShaderPass* shaderPass) noexcept;

	virtual void onBind  (RenderContext* ctx, RenderCommand_DrawCall& drawCall) = 0;
	virtual void onUnbind(RenderContext* ctx, RenderCommand_DrawCall& drawCall) = 0;

	template<class V>
	void _setParam(StrView name, const V& v) {
		if (_vertexStage)	_vertexStage->_setParam(name, v);
		if (_pixelStage)	_pixelStage->_setParam(name, v);
		if (_computeStage)  _computeStage->_setParam(name, v);
	}

	template<class V>
	void _setTexParam(StrView name, const V& v) {
		if (_vertexStage)	_vertexStage->_setTexParam(name, v);
		if (_pixelStage)	_pixelStage->_setTexParam(name, v);
		if (_computeStage)  _computeStage->_setTexParam(name, v);
	}

	template<class V>
	void _setStorageBufferParam(StrView name, const V& v) {
		if (_vertexStage)	_vertexStage->_setStorageBufferParam(name, v);
		if (_pixelStage)	_pixelStage->_setStorageBufferParam(name, v);
		if (_computeStage)  _computeStage->_setStorageBufferParam(name, v);
	}

	Material*	  _material		= nullptr;
	ShaderPass*   _shaderPass	= nullptr;
	VertexStage*  _vertexStage	= nullptr;
	PixelStage*   _pixelStage	= nullptr;
	ComputeStage* _computeStage = nullptr;
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
	using ComputeStage	= MaterialPass_ComputeStage;

	void setParam(StrView name, RenderGpuStorageBuffer* v) { _setStorageBufferParam(name, v); }

	void setParam(StrView name, Texture2D*     v) { _setTexParam(name, v); }
	void setParam(StrView name, TextureCube*   v) { _setTexParam(name, v); }

	void setParam(StrView name, const     i32& v) { _setParam(name, v); }
	void setParam(StrView name, const     u32& v) { _setParam(name, v); }
	void setParam(StrView name, const   float& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple2i& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple3i& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple4i& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple2f& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple3f& v) { _setParam(name, v); }
	void setParam(StrView name, const Tuple4f& v) { _setParam(name, v); }
	void setParam(StrView name, const   Mat4f& v) { _setParam(name, v); }
	void setParam(StrView name, const Color4f& v) { _setParam(name, v); }
	void setParam(StrView name, const Color4b& v) { _setParam(name, v); }

	void setParam(StrView name, const   RenderInt32x2Array& v) { _setParam(name, v); }
	void setParam(StrView name, const   RenderInt32x3Array& v) { _setParam(name, v); }
	void setParam(StrView name, const   RenderInt32x4Array& v) { _setParam(name, v); }
	void setParam(StrView name, const RenderFloat32x2Array& v) { _setParam(name, v); }
	void setParam(StrView name, const RenderFloat32x3Array& v) { _setParam(name, v); }
	void setParam(StrView name, const RenderFloat32x4Array& v) { _setParam(name, v); }
	void setParam(StrView name, const RenderFloat64x2Array& v) { _setParam(name, v); }
	void setParam(StrView name, const RenderFloat64x3Array& v) { _setParam(name, v); }
	void setParam(StrView name, const RenderFloat64x4Array& v) { _setParam(name, v); }

	void setShader(Shader* shader);

	Span< UPtr<Pass> >	passes() { return _passes; }

	Pass* getPass(int index) const {
		if (index >= _passes.size()) {
			SGE_ASSERT(false);
			return nullptr;
		}
		return _passes[index].get();
	}

	void bind(RenderContext& ctx, RenderCommand_DrawCall& drawCall);
	void unbind(RenderContext& ctx, RenderCommand_DrawCall& drawCall);

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

	template<class V>
	void _setStorageBufferParam(StrView name, const V& v) {
		for (auto& pass : _passes) {
			if (pass) pass->_setStorageBufferParam(name, v);
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