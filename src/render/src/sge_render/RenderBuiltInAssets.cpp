#include "RenderBuiltInAssets.h"
#include "Renderer.h"

namespace sge {

RenderBuiltInAssets* RenderBuiltInAssets::instance() {
	return Renderer::instance()->builtInAssets();
}

void RenderBuiltInAssets::load() {
	auto* p = Renderer::instance();
	StrView importedPath = ProjectSettings::instance()->importedPath();

	TempString tmp;
	#define E(NAME, ...) \
		{ \
			tmp.set(importedPath, "/Shaders/" #NAME ".shader"); \
			if (File::exists(tmp)) \
				shaders.NAME = p->createShader(tmp); \
		} \
	//----
		RenderBuiltInAssets_ShaderId_ENUM_LIST(E)
	#undef E

	{
		stockTextures.white		= p->createSolidColorTexture2D(Color4b(255, 255, 255, 255));
		stockTextures.black		= p->createSolidColorTexture2D(Color4b(0,   0,   0,   255));
		stockTextures.red		= p->createSolidColorTexture2D(Color4b(255, 0,   0,   255));
		stockTextures.green		= p->createSolidColorTexture2D(Color4b(0,   255, 0,   255));
		stockTextures.blue		= p->createSolidColorTexture2D(Color4b(0,   0,   255, 255));
		stockTextures.magenta	= p->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
		stockTextures.error		= p->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
	}

	textures.uvChecker = p->createTexture2DFromFile("Assets/Textures/uvChecker.png");
}

} // namespace sge
