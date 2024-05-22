#pragma once 

#include <any>

#include <Common.hpp>
#include <OS.Assert.hpp>

namespace Memory {

	using AllocationFunction = std::function<void* (
		void* userData,
		Common::Size    size,
		Common::Size    alignment)>;

	using ReallocationFunction = std::function<void* (
		void* userData,
		void* original,
		Common::Size    size,
		Common::Size    alignment)>;

	using FreeFunction = std::function<void(
		void* userData,
		void* memory)>;

	struct AllocationCallbacks {
		void*                userData_;

		AllocationFunction      allocationCallback_ = [](
			void* userData,
			Common::Size    size,
			Common::Size    alignment) {
				return std::malloc(size);
		};

		ReallocationFunction    reallocationCallback_ = [](
			void* userData,
			void* original,
			Common::Size    size,
			Common::Size    alignment) { OS::NotImplemented(); return nullptr; };

		FreeFunction            freeCallback_ = [](
			void* userData,
			void* memory) { std::free(memory); };

		template<class Type>
		Type* GetUserData() {
			return static_cast<Type*>(userData_);
		}

		template<class Type>
		void SetUserData(Type* userData) {
			userData_ = userData;
		}

		template<class Type>
		[[nodiscard]]
		Type* Allocate(Common::Size alignment = 8) {
			return static_cast<Type*>(allocationCallback_(userData_, sizeof(Type), alignment));
		}

		template<class Type>
		[[nodiscard]]
		Type* AllocateArray(Common::Size size, Common::Size alignment = 8) {
			return static_cast<Type*>(allocationCallback_(userData_, sizeof(Type) * size, alignment));
		}

		template<class Type, class ...Args>
		[[nodiscard]]
		Type* Create(Args&& ...args) {
			Type* const memory = Allocate<Type>();
			return Construct<Type>(memory, std::forward<Args>(args)...);
		}

		template<class Type, class ...Args>
		[[nodiscard]]
		Type* CreateArray(Common::Size size, Args&& ...args) {
			Type* const memory = AllocateArray<Type>(size);
			return ConstructArray<Type>(memory, size, std::forward<Args>(args)...);
		}

		template<class Type>
		void Free(Type* memory) noexcept {
			freeCallback_(userData_, memory);
		}

		template<class Type>
		void Destroy(Type* object) {
			Destruct<Type>(object);
			Free<Type>(object);
		}

		template<class Type>
		void DestroyArray(Type* memory, Common::Size size) {
			for (Common::Size i = 0; i < size; i++) {
				Destruct<Type>(memory + i);
			}
			Free<Type>(memory);
		}

	};

}