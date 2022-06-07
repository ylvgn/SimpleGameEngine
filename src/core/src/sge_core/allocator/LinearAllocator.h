#pragma once
#include "../math/Math.h"

namespace sge {

template<size_t CHUNK_SIZE>
struct LinearAllocator_Chunk : public NonCopyable {
	void* allocate(size_t reqSize, size_t align) {
		auto offset = Math::alignTo(_used, align);
		auto available = CHUNK_SIZE - offset - _used;
		if (available < reqSize) {
			return nullptr;
		}

		_used = offset + reqSize;
		return _buffer + offset;
	};

	void clear() { _used = 0; };

private:
	u8 _buffer[CHUNK_SIZE];
	size_t _used = 0;
};

template<>
struct LinearAllocator_Chunk<0> : public NonCopyable {
	// not allow use LinearAllocator_Chunk<0>
};

// --------------------------------------------
template<size_t CHUNK_SIZE>
class ILinearAllocator : LinearAllocator_Chunk<CHUNK_SIZE> {
	using Chunk = LinearAllocator_Chunk<CHUNK_SIZE>;
public:
	void* allocate(size_t reqSize, size_t align = 16) {
		if (_chunks.size()) {
			auto& t = _chunks.back();
			auto* p = t->allocate(reqSize, align);
			if (p) return p;
		}

		SGE_ASSERT(reqSize <= CHUNK_SIZE);

		auto newChunk = eastl::make_unique<Chunk>();
		_chunks.emplace_back(eastl::move(newChunk));
		auto* p = _chunks.back()->allocate(reqSize, align);
		return p;
	}

	void clear() {
		for (auto& c : _chunks) {
			c->clear();
		}
	}
private:
	Vector<UPtr<Chunk>>	_chunks;
}; // LinearAllocator


using LinearAllocator = ILinearAllocator<16 * 1024>;

} // namespace