//
// CES
// Resource Container
// 作者：SunnyCase
// 创建时间：2017-03-20
//
#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <Tomato.Core/NonCopyable.h>
#include <Tomato.Core/FreeList.h>
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>

namespace CES
{
	template<class T>
	class ResourceContainer
	{
	protected:
		struct ObjectStorage : NonCopyable
		{
			static_assert(std::is_nothrow_destructible<T>::value, "T must be nothrow destructible.");

			ObjectStorage() noexcept
				:Inited(false)
			{

			}

			~ObjectStorage()
			{
				if (Inited)
					Free();
			}

			ObjectStorage(ObjectStorage&& other) noexcept
				:Inited(other.Inited), Used(other.Used)
			{
				if (Inited)
				{
					new (_data) T(std::move(other.GetObject()));
					other.Free();
				}
			}

			ObjectStorage& operator=(ObjectStorage&& other) noexcept
			{
				if (Inited)
					Free();
				if (other.Inited)
				{
					new (_data) T(std::move(other.GetObject()));
					Inited = true;
					Used = other.Used;
					other.Free();
				}
				return *this;
			}

			template<class ...P>
			void Create(P&&... params)
			{
				if (Inited)
					Free();
				new (_data) T(std::forward<P>(params)...);
				Inited = true;
			}

			T& GetObject()
			{
				assert(Inited);
				return *reinterpret_cast<T*>(_data);
			}

			const T& GetObject() const
			{
				assert(Inited);
				return *reinterpret_cast<T const*>(_data);
			}

			void Free() noexcept
			{
				GetObject().~T();
				Inited = false;
				Used = false;
			}

			bool GetUsed() const noexcept { return Used; }

			void SetUsed() noexcept
			{
				Used = true;
			}

			void SetUnused() noexcept
			{
				Used = false;
			}

		private:
			alignas(alignof(T)) char _data[sizeof(T)];

			bool Used = false;
			bool Inited;
		};
	public:
		ResourceContainer(size_t capacity = 231)
			:_freeList(capacity)
		{
			_data.resize(capacity);
		}

		UINT_PTR Allocate()
		{
			size_t handle;
			auto ret = _freeList.TryAllocate(1, handle);
			if (!ret)
			{
				Enlarge(231);
				ret = _freeList.TryAllocate(1, handle);
			}
			assert(ret);
			auto& obj = _data.at(handle);
			obj.Create();
			obj.SetUsed();
			return handle;
		}

		void Enlarge(size_t up = 0)
		{
			const auto oldCapacity = _data.size();
			const auto upCapacity = up ? up : oldCapacity;
			const auto newCapacity = oldCapacity + upCapacity;
			_data.resize(newCapacity);
			_freeList.Enlarge(up);
		}

		HRESULT RetireResource(UINT_PTR handle) noexcept
		{
			try
			{
				auto& obj = _data[handle];
				obj.Free();
				obj.SetUnused();
				_cleanupList.emplace_back(size_t(handle));
				return S_OK;
			}
			TCATCH_ALL();
		}

		void RetireAndCleanupResource(UINT_PTR handle)
		{
			auto& obj = _data[handle];
			obj.Free();
			obj.SetUnused();
			_freeList.Retire(handle, 1);
		}

		void CleanUp()
		{
			for (auto&& handle : _cleanupList)
				_freeList.Retire(handle, 1);
			_cleanupList.clear();
		}

		T const & FindResource(UINT_PTR handle) const noexcept
		{
			auto& value = _data[handle];
			assert(value.GetUsed());
			return value.GetObject();
		}

		T & FindResource(UINT_PTR handle) noexcept
		{
			auto& value = _data[handle];
			assert(value.GetUsed());
			return value.GetObject();
		}

		bool TryFindResource(UINT_PTR handle, T const*& refer) const noexcept
		{
			auto& value = _data[handle];
			if (value.GetUsed())
			{
				refer = &value.GetObject();
				return true;
			}
			return false;
		}

		const std::vector<UINT_PTR> GetCleanupList() const noexcept
		{
			return _cleanupList;
		}
	protected:
		std::vector<ObjectStorage> _data;
		NS_CORE::FreeList _freeList;
		std::vector<UINT_PTR> _cleanupList;
	};
}