#pragma once

namespace sge {
	
struct NeHeOGL_Image {
	int width = 0;
	int height = 0;

	Vector<u8> pixelData;

	void clean() {
		width = 0;
		height = 0;
		pixelData.clear();
	}

	void loadFile(StrView filename) {
		auto ext = FilePath::extension(filename);
		if (0 == StringUtil::ignoreCaseCompare(ext, "bmp")) {
			return loadBmpFile(filename);
		}

		throw SGE_ERROR("unsupported image file format {}", ext);
	}

	void loadBmpFile(StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		loadBmpMem(mm);
	}

	void loadBmpMem(ByteSpan data);
};

}