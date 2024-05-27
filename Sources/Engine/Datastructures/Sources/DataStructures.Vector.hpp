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

		Vector(Vector&& moveVector) noexcept :
			allocationCallbacks_{ moveVector.allocationCallbacks_ },
			data_{ nullptr },
			size_{ 0 },
			capacity_{ 0 } {

			std::swap(data_, moveVector.data_);
			std::swap(size_, moveVector.size_);
			std::swap(capacity_, moveVector.capacity_);

		}

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

		Type& PushBack(const Type& object) {
			Resize(GetSize() + 1);
			return Get(GetSize() - 1) = object;
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
			return data_[GetSize() - 1];
		}

		const Type& Back() const {
			return *const_cast<Vector*>(this).Back();
		}

		[[nodiscard]]
		Type& Get(Common::Index index) {
			OS::AssertMessage(
				index < GetSize(),
				"Out of range.");
			return data_[index];
		}

		[[nodiscard]]
		const Type& Get(Common::Index index) const {
			return const_cast<Vector*>(this)->Get(index);
		}

		[[nodiscard]]
		Type& operator[](Common::Size index) {
			return Get(index);
		}

		[[nodiscard]]
		const Type& operator[](Common::Size index) const {
			return Get(index);
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

		Vector& operator=(const Vector& copyVector) {
			if (this == &copyVector) {
				return *this;
			}

			Clear();
			Resize(copyVector.GetSize());
			for (Common::Index i = 0; i < copyVector.GetSize(); i++) {
				Get(i) = copyVector.Get(i);
			}

			return *this;
		}

		void Erase(Common::Index index) {
			for (Common::Index i = index; i < GetSize() - 1; i++) {
				Get(i) = std::move(Get(i + 1));
			}
			Memory::Destruct(&Back());
			--size_;
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