#pragma once

#include "shader/Material.h"

namespace sge {

#define RenderBuiltInAssets_ShaderId_ENUM_LIST(E) \
	E(Unknown,) \
	E(BlinnPhong,) \
	E(Wireframe,) \
	E(Gizmo,) \
	E(UIText,) \
	E(UITexture0,) \
	E(UITexture1,) \
	E(UITexture2,) \
	E(UITextTexture0,) \
	E(UIColorMaskTexture,) \
	E(TextBillboardTexture0,) \
	E(UnlitTexture0,) \
	E(UnlitTexture1,) \
	E(UnlitTexture2,) \
	E(SimpleLighting,) \
//-------
SGE_ENUM_CLASS(RenderBuiltInAssets_ShaderId, u16)

class RenderBuiltInAssets : public NonCopyable {
public:
	static RenderBuiltInAssets* instance();

	void load();

	struct Shaders {
	#define E(NAME, ...) \
		SPtr<Shader> NAME; \
	//----
		RenderBuiltInAssets_ShaderId_ENUM_LIST(E)
	#undef E
	};

	Shaders shaders;

	struct StockTextures {
		SPtr<Texture2D>	white;
		SPtr<Texture2D>	black;
		SPtr<Texture2D>	red;
		SPtr<Texture2D>	green;
		SPtr<Texture2D>	blue;
		SPtr<Texture2D>	magenta;
		SPtr<Texture2D>	error;
	};
	StockTextures stockTextures;

	struct Textures {
		SPtr<Texture2D>	uvChecker;
	};
	Textures textures;
};

} // namespace sge