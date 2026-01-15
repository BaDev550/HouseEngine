#pragma once
#include <iostream>
#include <memory>

namespace MEM {
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename... Args>
	Scope<T> MakeScope(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }

	class RefCounted {
	public:
		virtual ~RefCounted() = default;
		void IncRefCount() const { _RefCount++; }
		void DecRefCount() const { _RefCount--; }
		uint32_t GetRefCount() const { return _RefCount; }
	private:
		mutable uint32_t _RefCount = 0;
	};

	template<typename T>
	class Ref {
	public:
		Ref() : _Instance(nullptr) {}
		Ref(std::nullptr_t null) : _Instance(nullptr) {}
		Ref(T* instance) : _Instance(instance) {
			//static_assert(std::is_base_of<RefCounted, T>::value, "Class in not RefCounted");
			//IncRef();
		}
		template<typename T2>
		Ref(const Ref<T2>& other) {
			_Instance = (T*)other._Instance;
			IncRef();
		}
		template<typename T2>
		Ref(Ref<T2>&& other) {
			_Instance = (T*)other._Instance;
			other._Instance = nullptr;
		}
		Ref(const Ref<T>& other) : _Instance(other._Instance) {
			IncRef();
		}
		~Ref() { DecRef(); }

		Ref& operator=(std::nullptr_t)
		{
			DecRef();
			_Instance = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other)
		{
			if (this == &other)
				return *this;

			other.IncRef();
			DecRef();

			_Instance = other._Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(const Ref<T2>& other)
		{
			other.IncRef();
			DecRef();

			_Instance = other._Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(Ref<T2>&& other)
		{
			DecRef();

			_Instance = other._Instance;
			other._Instance = nullptr;
			return *this;
		}

		operator bool() { return _Instance != nullptr; }
		operator bool() const { return _Instance != nullptr; }

		T* operator->() { return _Instance; }
		const T* operator->() const { return _Instance; }

		T& operator*() { return *_Instance; }
		const T& operator*() const { return *_Instance; }

		T* Raw() { return  _Instance; }
		const T* Raw() const { return  _Instance; }

		void Reset(T* instance = nullptr)
		{
			DecRef();
			_Instance = instance;
		}

		template<typename T2>
		Ref<T2> As() const
		{
			return Ref<T2>(*this);
		}

		template<typename... Args>
		static Ref<T> Create(Args&&... args)
		{
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		bool operator==(const Ref<T>& other) const
		{
			return _Instance == other._Instance;
		}

		bool operator!=(const Ref<T>& other) const
		{
			return !(*this == other);
		}
	private:
		void IncRef() const {
			//_Instance->IncRefCount();
		}
		void DecRef() const {
			if (_Instance) {
				//_Instance->IncRefCount();
				//if (_Instance->GetRefCount() == 0) {
				//	delete _Instance;
				//	_Instance = nullptr;
				//}
			}
		}
	private:
		template<typename T2>
		friend class Ref;
		mutable T* _Instance;
	};
}