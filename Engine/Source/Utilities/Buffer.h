#pragma once

#include <stdint.h>
#include <cstring>
#include <iostream>

struct Buffer
{
	uint8_t* Data = nullptr;
	uint64_t Size = 0;

	Buffer() = default;

	Buffer(uint64_t size)
	{
		Allocate(size);
	}

	Buffer(const void* data, uint64_t size)
		: Data((uint8_t*)data), Size(size)
	{
	}

	Buffer(const Buffer&) = default;

	static Buffer Copy(Buffer other)
	{
		Buffer result(other.Size);
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
	T* As()
	{
		return (T*)Data;
	}

	operator bool() const
	{
		return (bool)Data;
	}

};

struct ScopedBuffer
{
	ScopedBuffer(Buffer buffer)
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
	Buffer _Buffer;
};