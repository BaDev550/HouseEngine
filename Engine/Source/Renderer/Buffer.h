#pragma once
#include "Utilities/Memory.h"
#include "Utilities/Flag.h"
#include "Utilities/Defines.h"

namespace House {
	enum class BufferType : uint8_t {
		None = 0,
		UniformBuffer = BIT(0),
		VertexBuffer  = BIT(1),
		IndexBuffer   = BIT(2),
		TransferDst   = BIT(3),
		TransferSrc   = BIT(4),
		StorageBuffer = BIT(5)
	};

	enum class MemoryProperties : uint8_t {
		None = 0,
		HOST_VISIBLE  = BIT(0),
		HOST_COHERENT = BIT(1),
		DEVICE        = BIT(2)
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

#if 0
	class UniformBuffer : public Buffer {
	public:
		static MEM::Ref<Buffer> Create(uint64_t size);
	};
#endif
}