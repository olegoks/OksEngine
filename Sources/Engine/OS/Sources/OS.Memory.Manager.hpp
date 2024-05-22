#pragma once

#include <memory>
#include <source_location>

#include <Common.hpp>
#include <OS.hpp>

#include <Memory.hpp>
#include <Memory.SharedPointer.hpp>
#include <Memory.BlockAllocator.hpp>

namespace Memory {

	class Manager {
	private:
		
		using Allocator = StdCompatible::DynamicPoolAllocator<Common::Byte>;

	public:
		Manager(Common::Size memoryPoolSize) noexcept :
			allocator_{ memoryPoolSize } { }

		template<class Type, class ... Args>
		auto MakeShared(Args&& ... args) noexcept {
			Memory::StdCompatible::DynamicPoolAllocator<Type> objectAllocator = allocator_;
			return SharedPointer<Type, Memory::StdCompatible::DynamicPoolAllocator<Type>>::Make(objectAllocator, std::forward<Args&&>(args)...);
		}

		template<class Type>
		Type* Allocate(Common::Size objectsNumber) noexcept {
			return reinterpret_cast<Type*>(allocator_.allocate(sizeof(Type) * objectsNumber));
		}

		void Deallocate(void* pointer) noexcept {
			allocator_.deallocate(reinterpret_cast<Common::Byte*>(pointer));
		}

	private:
		 Allocator allocator_;
	};
	
	class TagedManager : public Manager {
	public:

		class [[nodiscard]] Tag final {
		private:

			struct Hash {
				[[nodiscard]]
				Common::UInt64 operator()(const Tag& tag) const noexcept {
					std::hash<const char*> hasher;
					return hasher(tag.tag_);
				}
			};

		public:

			Tag(const char* tag) noexcept : 
				tag_{ tag },
				hash_{ Hash()(*this) } {
				OS::AssertMessage(IsValidTagString(tag),
					"Attempt to use invalid tag string.");
			}

			Tag(const Tag& copyTag) noexcept : 
				tag_{ copyTag.tag_ },
				hash_{ copyTag.hash_ } { }

			operator const char* () const noexcept {
				return GetString();
			}

			[[nodiscard]]
			const char* GetString() const noexcept {
				return tag_;
			}

		private:
			[[nodiscard]]
			bool IsValidTagString(const char* tag) noexcept {
				//OS::NotImplemented();
				return true;
			}

		private:
			Common::UInt64 hash_ = 0;
			const char* tag_ = nullptr;
		};

		TagedManager(Common::Size memoryPoolSize) noexcept :
			Manager{ memoryPoolSize } { }

		template<class Type, class ... Args>
		SharedPointer<Type> MakeShared(Tag&& tag, Args&& ... args) noexcept {

			return Manager::MakeShared<Type>(std::forward<Args&&>(args)...);
		}

		template<class Type, class ...Args>
		Type* Create(Tag&& tag, Args&& ... args) noexcept {
			Type* object = Allocate<Type>(tag, std::forward<Args&&>(args)...);
			Construct<Type>(object, std::forward<Args&&>(args)...);
			return object;
		}

		template<class Type>
		Type* Allocate(Tag&& tag, Common::Size size) noexcept {
			return Manager::Allocate<Type>(size);
		}

		void Deallocate(void* pointer) noexcept {
			Manager::Deallocate(pointer);
		}
	};

	[[nodiscard]]
	TagedManager::Tag operator""_tag(const char* tagString) noexcept {
		return TagedManager::Tag{ tagString };
	}


	TagedManager memory{ 64_MB };

}