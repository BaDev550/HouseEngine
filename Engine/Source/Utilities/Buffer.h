#pragma once

#include <stdint.h>
#include <cstring>
#include <iostream>

struct DataBuffer
{
	uint8_t* Data = nullptr;
	uint64_t Size = 0;

	DataBuffer() = default;

	DataBuffer(uint64_t size)
	{
		Allocate(size);
	}

	DataBuffer(const void* data, uint64_t size)
		: Data((uint8_t*)data), Size(size)
	{
	}

	DataBuffer(const DataBuffer&) = default;

	static DataBuffer Copy(DataBuffer other)
	{
		DataBuffer result(other.Size);
		memcpy(result.Data, other.Data, other.Size);
		return result;
	}

	void Allocate(uint64_t size)
	{
		Release();

		Data = (uint8_t*)malloc(size);
		Size = size;
	}

	void Release()
	{
		free(Data);
		Data = nullptr;
		Size = 0;
	}

	template<typename T>
	T& Read(uint64_t offset = 0)
	{
		return *(T*)((uint8_t*)Data + offset);
	}

	template<typename T>
	const T& Read(uint64_t offset = 0) const
	{
		return *(T*)((uint8_t*)Data + offset);
	}

	template<typename T>
	T* As()
	{
		return (T*)Data;
	}

	operator bool() const
	{
		return (bool)Data;
	}

	void Write(const void* data, uint64_t size, uint64_t offset = 0)
	{
		memcpy((uint8_t*)Data + offset, data, size);
	}

	void ZeroInitialize()
	{
		if (Data)
			memset(Data, 0, Size);
	}
};

struct ScopedBuffer
{
	ScopedBuffer(DataBuffer buffer)
		: _Buffer(buffer)
	{
	}

	ScopedBuffer(uint64_t size)
		: _Buffer(size)
	{
	}

	~ScopedBuffer()
	{
		_Buffer.Release();
	}

	uint8_t* Data() { return _Buffer.Data; }
	uint64_t Size() { return _Buffer.Size; }

	template<typename T>
	T* As()
	{
		return _Buffer.As<T>();
	}

	operator bool() const { return _Buffer; }
private:
	DataBuffer _Buffer;
};