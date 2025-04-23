#include "Texture.h"

namespace sge {

#if 0
#pragma mark ========= Texture2D ============
#endif
Texture2D::Texture2D(CreateDesc& desc) {
	_type			= DataType::Texture2D;
	_colorType		= desc.colorType;
	_mipmapCount	= desc.mipmapCount;
	_size			= desc.size;
	_samplerState	= desc.samplerState;
}


#if 0
#pragma mark ========= TextureCube ============
#endif
TextureCube::TextureCube(CreateDesc& desc) : Base(desc) {
	_type = DataType::TextureCube;
}

} // namespace sge

