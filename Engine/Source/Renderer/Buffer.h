#pragma once
#include "Utilities/Memory.h"
#include "Utilities/Flag.h"

namespace House {
	enum class BufferType : uint8_t {
		UniformBuffer,
		VertexBuffer,
		IndexBuffer,
		TransferDst,
		TransferSrc,
	};

	enum class MemoryProperties : uint8_t {
		HOST_VISIBLE,
		HOST_COHERENT,
		DEVICE
	};

	template <>
	struct FlagTraits<BufferType> { static constexpr bool is_bitmask = true; };
	using BufferFlags = Flags<BufferType>;

	template <>
	struct FlagTraits<MemoryProperties> { static constexpr bool is_bitmask = true; };
	using MemoryFlags = Flags<MemoryProperties>;

	class Buffer : public MEM::RefCounted {
	public:
		virtual void Map(uint64_t size = UINT64_MAX, uint64_t offset = 0) {}
		virtual void Unmap() {}

		virtual void WriteToBuffer(void* data, uint64_t size = UINT64_MAX, uint64_t offset = 0) = 0;
		virtual bool Invalidate(uint64_t size = UINT64_MAX, uint64_t offset = 0) = 0;
		virtual void* GetData() const = 0;

		static MEM::Ref<Buffer> Create(uint64_t size, BufferFlags typeFlags, MemoryFlags memprops = MemoryProperties::HOST_VISIBLE);
	};

	class UniformBuffer : public Buffer {
	public:
		static MEM::Ref<Buffer> Create(uint64_t size);
	};
}