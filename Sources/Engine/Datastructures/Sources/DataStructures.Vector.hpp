#pragma once

#include <algorithm>

#include <OS.Memory.Common.hpp>
#include <OS.Memory.AllocationCallbacks.hpp>

namespace Datastructures {

	template<class Type>
	class [[nodiscard]] Vector {
	public:
		Vector(Memory::AllocationCallbacks allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
			allocationCallbacks_{ allocationCallbacks } {}

		void Resize(Common::Size newSize) {
			if (newSize == GetCapacity()) { return; }
			Type* const newData = allocationCallbacks_.CreateArray<Type>(newSize);
			Common::UInt64 objectsToCopyNumber = GetSize();
			if (newSize < objectsToCopyNumber) { objectsToCopyNumber = newSize; }
			for (Common::Index i = 0; i < objectsToCopyNumber; i++) {
				newData[i] = data_[i];
			}
			if (GetData() != nullptr) {
				allocationCallbacks_.DestroyArray<Type>(GetData(), GetSize());
			}
			data_ = newData;
			size_ = newSize;
		}

		void Reserve(Common::Size newCapacity) {
			if (newCapacity <= GetCapacity()) { return; }
			Type* const newData = allocationCallbacks_.CreateArray<Type>(newCapacity);
			for (Common::Index i = 0; i < GetSize(); i++) {
				newData[i] = data_[i];
			}
			allocationCallbacks_.DestroyArray<Type>(GetData(), GetSize());
			data_ = newData;
			capacity_ = newCapacity;
		}

		Type& PushBack(Type&& object) {
			return EmplaceBack(std::move(object));
		}
		 
		template<class ...Args>
		Type& EmplaceBack(Args&& ...args) {
			Reserve(GetSize() + 1);
			Type* const object = Construct<Type>(GetData() + GetSize(), std::forward<Args>(args)...);
			++size_;
			return *object;
		}

		Type& Back() {
			return *const_cast<Vector*>(this).Back();
		}

		const Type& Back() const {
			return data_[GetSize() - 1];
		}

		[[nodiscard]]
		Type& operator[](Common::Size index) {
			OS::AssertMessage(
				index < GetSize(),
				"Incorrect index.");
			return data_[index];
		}

		[[nodiscard]]
		const Type& operator[](Common::Size index) const {
			return *const_cast<Vector*>(this)[index];
		}

		[[nodiscard]]
		Common::Size GetSize() const noexcept {
			return size_;
		}

		[[nodiscard]]
		Common::Size GetCapacity() const noexcept {
			return capacity_;
		}

		[[nodiscard]]
		const Type* GetData() const {
			return const_cast<Vector*>(this)->GetData();
		}

		[[nodiscard]]
		Type* GetData() noexcept {
			return data_;
		}

		void Clear() {
			Memory::DestructArray<Type>(GetData(), GetSize());
			size_ = 0;
		}

		void Free() noexcept {
			Clear();
			allocationCallbacks_.Free<Type>(GetData());
			capacity_ = 0;
		}

		~Vector() noexcept {
			Free();
		}

	private:
		Memory::AllocationCallbacks allocationCallbacks_;
		Type* data_ = nullptr;
		Common::Size size_ = 0;
		Common::Size capacity_ = 0;
	};


}

namespace DS = Datastructures;