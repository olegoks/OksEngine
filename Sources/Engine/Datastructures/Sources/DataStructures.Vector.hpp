#pragma once

#include <algorithm>

#include <OS.Memory.Common.hpp>
#include <OS.Memory.AllocationCallbacks.hpp>

#include <boost/thread/shared_mutex.hpp>

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

		Vector(const Vector& copyVector) :
			allocationCallbacks_{ copyVector.allocationCallbacks_ },
			data_{ nullptr },
			size_{ 0 },
			capacity_{ 0 } {

			Resize(copyVector.size_);
			for (Common::Index i = 0; i < GetSize(); i++) {
				data_[i] = copyVector[i];
			}
		}

		void Resize(Common::Size newSize) {
			Reserve(2 * newSize);
			if(newSize <= GetSize()) {
				Memory::DestructArray<Type>(data_ + newSize, GetSize() - newSize);
			} else {
				Memory::ConstructArray<Type>(data_ + GetSize(), newSize - GetSize());
			}
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
			Type* const object = Memory::Construct<Type>(GetData() + GetSize(), std::forward<Args>(args)...);
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

		[[nodiscard]]
		Common::Index Find(const Type& value) {
			for (Common::Index i = 0; i < GetSize(); i++) {
				if (value == Get(i)) {
					return i;
				}
			}
			//Not found.
			return GetSize() + 1;
		}

		void Erase(const Type& value) {
			Common::Index valueIndex = Find(value);
			if (valueIndex != GetSize() + 1) {
				Erase(valueIndex);
			}
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

	template<class Type>
	class ThreadSafeVector {
	public:

		void PushBack(const Type& copyValue) {
			std::lock_guard guard{ mutex_ };
			vector_.PushBack(copyValue);
		}

		void Erase(Common::Index index) {
			std::lock_guard guard{ mutex_ };
			vector_.Erase(index);
		}

		[[nodiscard]]
		Common::Size GetSize() {
			std::lock_guard guard{ mutex_ };
			return vector_.GetSize();
		}

		[[nodiscard]]
		Type Get(Common::Index index) {
			return vector_.Get(index);
		}

	private:
		boost::shared_mutex mutex_;
		Vector<Type> vector_;
	};

}

namespace DS = Datastructures;