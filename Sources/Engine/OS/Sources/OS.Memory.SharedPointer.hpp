#pragma once

#include <utility>

#include <Common.hpp>
#include <OS.hpp>

#include <Memory.BlockAllocator.hpp>

namespace Memory {

	template<class Type, class Allocator = Memory::StdCompatible::DynamicPoolAllocator<Type>>
	class SharedPointer {
	private:

		template<class Type, class Allocator>
		class ControlBlock final {
		public:

			template<class ...Args>
			explicit ControlBlock(Allocator& allocator, Args&& ...args) noexcept : 
				allocator_{ allocator } {
				Type* objectMemory = allocator_.allocate();
				allocator_.construct(objectMemory, std::forward<Args&&>(args)...);
				data_ = objectMemory;
			}

			[[nodiscard]]
			Common::Size GetLinksNumber() const noexcept {
				return linksNumber_;
			}

			void AddLink() noexcept {
				OS::AssertMessage(GetLinksNumber() >= 1, "Invalid links number. It must be more than 0.");
				++linksNumber_;
			}

			void RemoveLink() noexcept {
				OS::AssertMessage(GetLinksNumber() >= 1, "To remove reference counter must be more than 0.");
				--linksNumber_;
			}

			[[nodiscard]]
			const Type* GetObject() const noexcept {
				return const_cast<ControlBlock<Type>*>(this)->GetObject();
			}

			[[nodiscard]] 
			Type* GetObject() noexcept {
				OS::AssertMessage(data_ != nullptr, "Invalid data pointer.");
				return data_;
			}

			~ControlBlock() noexcept {
				OS::AssertMessage(GetLinksNumber() == 0, 
					"Attempt to destroy control block when there are more than one link.");
				allocator_.deallocate(GetObject());
				allocator_.destroy(GetObject());
			}

			[[nodiscard]]
			Allocator& GetAllocator() noexcept {
				return allocator_;
			}

		private:
			Allocator allocator_;
			Common::Size linksNumber_ = 1;
			Type* data_ = nullptr;
		};

	public:

		SharedPointer(Type* pointer = nullptr) noexcept : controlBlock_{ nullptr } {
		
			OS::AssertMessage(pointer == nullptr, "Class doesn't support using raw pointers.");
			
		}

		template<class ...Args>
		SharedPointer(Allocator& allocator, Args&& ...args) noexcept :
			controlBlock_{ nullptr } {

			Memory::StdCompatible::DynamicPoolAllocator<ControlBlock<Type, Allocator>> controlBlockAllocator = allocator;
			controlBlock_ = controlBlockAllocator.allocate();
			controlBlockAllocator.construct(controlBlock_, allocator, std::forward<Args&&>(args)...);

		}

		SharedPointer(const SharedPointer& copyPointer) noexcept :
			controlBlock_{ copyPointer.controlBlock_ } {

			controlBlock_->AddLink();
			
		}

		[[nodiscard]]
		bool operator==(Type* pointer) const noexcept {

			const bool bothNullptr = (controlBlock_ == nullptr) && (pointer == nullptr);
			if (bothNullptr) {
				return true;
			}

			const bool equalPointers = (controlBlock_ != nullptr) && (controlBlock_->GetObject() == pointer);
			if (equalPointers) {
				return true;
			}

			return false;
		}

		operator bool() noexcept {
			return (controlBlock_ != nullptr);
		}

		SharedPointer& operator=(const SharedPointer& copyPointer) noexcept {

			if (this == &copyPointer) {
				return *this;
			}

			if (controlBlock_ != nullptr) {
				controlBlock_->RemoveLink();
				if (controlBlock_->GetLinksNumber() == 0) {
					Memory::StdCompatible::DynamicPoolAllocator<ControlBlock<Type, Allocator>> controlBlockAllocator = controlBlock_->GetAllocator();
					controlBlockAllocator.destroy(controlBlock_);
					controlBlockAllocator.deallocate(controlBlock_);
				}
				controlBlock_ = nullptr;
			}

			controlBlock_ = copyPointer.controlBlock_;
			controlBlock_->AddLink();

			return *this;

		}

		[[nodiscard]]
		Type& operator*() noexcept {
			
			OS::AssertMessage(controlBlock_ != nullptr, "Invalid state of smart pointer.");
			OS::AssertMessage(controlBlock_->GetLinksNumber() >= 1, "Invalid links number value.");
			OS::AssertMessage(Get() != nullptr, "Control block doesn't contain valid pointer.");

			return *Get();

		}

		[[nodiscard]]
		Type* operator->() noexcept {

			OS::AssertMessage(controlBlock_ != nullptr, "Invalid state of smart pointer.");
			OS::AssertMessage(controlBlock_->GetLinksNumber() >= 1, "Invalid links number value.");
			OS::AssertMessage(Get() != nullptr, "Control block doesn't contain valid pointer.");

			return Get();

		}

		[[nodiscard]]
		Type* Get() noexcept {

			OS::AssertMessage(controlBlock_ != nullptr, "Invalid state of smart pointer.");
			OS::AssertMessage(controlBlock_->GetLinksNumber() >= 1, "Invalid links number value.");

			return controlBlock_->GetObject();
		}

		~SharedPointer() noexcept {
			
			if (controlBlock_ != nullptr) {
				OS::AssertMessage(controlBlock_->GetObject() != nullptr,
					"Invalid value of data pointer.");
				OS::AssertMessage(controlBlock_->GetLinksNumber() >= 1,
					"Invalid value of number of links.");
				controlBlock_->RemoveLink();
				if (controlBlock_->GetLinksNumber() == 0) {
					Memory::StdCompatible::DynamicPoolAllocator<ControlBlock<Type, Allocator>> controlBlockAllocator = controlBlock_->GetAllocator();
					controlBlockAllocator.destroy(controlBlock_);
					controlBlockAllocator.deallocate(controlBlock_);
				}
			}

		}

		template<class ...Args>
		[[nodiscard]]
		static SharedPointer Make(Allocator& allocator, Args&&... args) noexcept {

			SharedPointer pointer{ allocator, std::forward<Args&&>(args)... };
			return pointer;
		}

	private:

		//void DestroyData() noexcept {
		//	OS::AssertMessage(controlBlock_ != nullptr, "Invalid state of control block.");
		//	OS::AssertMessage(controlBlock_->GetLinksNumber() == 0, "Attempt to delete control block with links.");
		//	allocator_.destroy(controlBlock_->GetData());
		//	allocator_.deallocate(controlBlock_->GetData());
		//}

		void DestroyControlBlock() {
			OS::AssertMessage(controlBlock_ != nullptr, "Invalid state of control block.");
			OS::AssertMessage(controlBlock_->GetLinksNumber() == 0, "Attempt to delete control block with links.");
			Memory::StdCompatible::DynamicPoolAllocator<ControlBlock<Type>> controlBlockAllocator = controlBlock_->GetAllocator();
			controlBlockAllocator.destroy(controlBlock_);
			controlBlockAllocator.deallocate(controlBlock_);
			controlBlock_ = nullptr;
		}

	private:
		ControlBlock<Type, Allocator>* controlBlock_ = nullptr;
	};

}