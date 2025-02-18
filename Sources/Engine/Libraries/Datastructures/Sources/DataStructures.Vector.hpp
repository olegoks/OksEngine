#pragma once

#include <algorithm>

#include <OS.Memory.Common.hpp>
#include <OS.Memory.AllocationCallbacks.hpp>

#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace DataStructures {

	template<class Type>
	class [[nodiscard]] Vector {
	public:
		Vector(Memory::AllocationCallbacks allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
			allocationCallbacks_{ allocationCallbacks } {}

		Vector(Vector&& moveVector) noexcept :
			allocationCallbacks_{ Memory::AllocationCallbacks{} },
			data_{ nullptr },
			size_{ 0 },
			capacity_{ 0 } {

			//std::swap(allocationCallbacks_, moveVector.allocationCallbacks_);
			std::swap(data_, moveVector.data_);
			std::swap(size_, moveVector.size_);
			std::swap(capacity_, moveVector.capacity_);

		}

		Vector(const Type* data, 
			Common::Size size,
			Memory::AllocationCallbacks allocationCallbacks = Memory::AllocationCallbacks{}) : 
			allocationCallbacks_{ allocationCallbacks },
			data_{ nullptr },
			size_{ 0 },
			capacity_{ 0 } {

			Resize(size);
			for (Common::Index i = 0; i < GetSize(); i++) {
				data_[i] = data[i];
			}

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
			OS::Assert(index < GetSize());
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
			if (data_ == nullptr) { return; }
			Clear();
			allocationCallbacks_.Free<Type>(GetData());
			data_ = nullptr;
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

		Vector& operator=(Vector&& moveVector) noexcept {
			if (this == &moveVector) {
				return *this;
			}

			Free();
			std::swap(allocationCallbacks_, moveVector.allocationCallbacks_);
			std::swap(data_, moveVector.data_);
			std::swap(size_, moveVector.size_);
			std::swap(capacity_, moveVector.capacity_);

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

	namespace ThreadSafe {
		template<class Type>
		class Vector {
		public:

			void PushBack(Type value) {
				std::lock_guard guard{ mutex_ };
				vector_.PushBack(value);
			}

			[[nodiscard]]
			bool Get(Type& outValue, Common::Index index) {
				boost::shared_lock guard{ mutex_ };
				if (index < vector_.GetSize()) {
					outValue = vector_[index];
					return true;
				}
				return false;
			}

			[[nodiscard]]
			Common::Size  GetSize() const noexcept {
				boost::shared_lock guard{ mutex_ };
				return vector_.GetSize();
			}

			[[nodiscard]]
			bool Contains(/*Type& outValue, */std::function<bool(const Type& value)> filter) const {
				boost::shared_lock lock{ mutex_ };
				for (Common::Index i = 0; i < vector_.GetSize(); i++) {
					if(filter(vector_[i])) {
						/*outValue = vector_[i];*/
						return true;
					}
				}
				return false;
			}

			void Erase(Common::Index index) {
				std::lock_guard guard{ mutex_ };
				if (index < vector_.GetSize()) {
					vector_.Erase(index);
				}
			}

		private:
			mutable boost::shared_mutex mutex_;
			DataStructures::Vector<Type> vector_;
		};
	}
	namespace TS = ThreadSafe;
}

namespace DS = DataStructures;