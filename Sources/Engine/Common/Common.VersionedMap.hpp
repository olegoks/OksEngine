#pragma once

#include <vector>

#include <memory>
#include <functional>
#include <compare>

#include <Common.Identifier.hpp>
#include <Common.Types.hpp>
#include <Common.Memory.Allocator.hpp>
#include <OS.Assert.hpp>

namespace Common {

	template<class Type, Memory::Allocator allocator = Memory::PoolAllocator<Type>>
	class [[nodiscard]] VersionedMap final {

	private:

		class [[nodiscard]] Slot final {
		public:

			using Version = Common::UInt32;

			constexpr static inline Common::UInt32 invalidVersion_ = Common::Limits<Common::UInt32>::Max();

			[[nodiscard]]
			bool IsFree() const noexcept {
				return data_ == nullptr;
			}

			[[nodiscard]]
			Version GetVersion() const noexcept { 
				return version_;
			}

			Version version_ = 0;
			Type* data_ = nullptr;
		};
		using SlotIndex = Common::UInt32;

	public:

		VersionedMap() noexcept {
			Slot freeSlot;
			{
				freeSlot.version_ = 0;
				freeSlot.data_ = nullptr;
			}
			slots_.resize(32, freeSlot);
		}

		class [[nodiscard]] Id {
		public:
			//union {
				//struct {
					Common::UInt32 slotIndex_;
					Common::UInt32 version_;
				//};
				//Common::UInt64 value_;
			//};

			struct Hash {
				Common::UInt64 operator()(const Id& id) const noexcept {
					std::hash<Common::UInt64> hasher;

						union {
							struct {
								Common::UInt32 slotIndex_;
								Common::UInt32 version_;
							};
							Common::UInt64 value_;
						} parsed;
						
						parsed.slotIndex_ = id.slotIndex_;
						parsed.version_ = id.version_;

					return hasher(parsed.value_);
				}
			};

			[[nodiscard]]
			bool operator==(const Id& id) const noexcept {
				return (slotIndex_ == id.slotIndex_) && (version_ == id.version_);
			}

			[[nodiscard]]
			bool operator<(const Id& id) const noexcept {
				return true;
			}

			
		};

		void DeleteElement(Id id) {
			Slot& slot = GetSlot(id);
			OS::AssertMessage(!slot.IsFree(), "Attempt to get data of free slot.");
			allocator_.destroy(slot.data_);
			allocator_.deallocate(slot.data_);
			slot.data_ = nullptr;
		}

		void Clear() {
			for (Slot& slot : slots_) {
				if (!slot.IsFree()) {
					allocator_.destroy(slot.data_);
					allocator_.deallocate(slot.data_);
					slot.data_ = nullptr;
				}
			}
		}

		Id AddElement(Type&& args) noexcept {

			Type* dataPointer = allocator_.allocate(1);
			allocator_.construct(dataPointer, std::move(args));

			const SlotIndex slotIndex = FindFreeSlot();
			Slot& slot = GetSlot(slotIndex);
			OS::AssertMessage(slot.data_ == nullptr, "Attempt to use not free slot.");
			{
				slot.data_ = dataPointer;
				slot.version_++;
			}
			Id slotId;
			{
				slotId.slotIndex_ = slotIndex;
				slotId.version_ = slot.version_;
			}
			return slotId;
		}

		[[nodiscard]]
		bool IsExists(Id id) noexcept {
			Slot& slot = GetSlot(id);
			const bool sameVersion = (slot.version_ == id.version_);
			return sameVersion && !slot.IsFree();
		}

		Type& GetElement(Id id) noexcept {
			Slot& slot = GetSlot(id);
			OS::AssertMessage(!slot.IsFree(), "Attempt to get data of free slot.");
			return *slot.data_;
		}

		using ProcessElement = std::function<void(Id, Type&)>;

		void ForEachElement(ProcessElement&& processor) noexcept {
			for (SlotIndex slotIndex = 0; slotIndex < slots_.size(); slotIndex++) {
				Slot& slot = slots_[slotIndex];
				if (!slot.IsFree()) {
					Id slotId{ slotIndex, slot.version_ };
					processor(slotId, *slot.data_);
				}
			}
		}

	private:

		Slot& GetSlot(Id id) {
			return GetSlot(id.slotIndex_);
		}

		Slot& GetSlot(SlotIndex index) {
			return slots_[index];
		}

		SlotIndex FindFreeSlot() noexcept {
			for (SlotIndex slotIndex = 0; slotIndex < slots_.size(); slotIndex++) {
				if (GetSlot(slotIndex).IsFree()) {
					return slotIndex;
				}
			}
			OS::AssertFailMessage("There are no free slots.");
			return Common::Limits<SlotIndex>::Max();
		}

		std::vector<Slot> slots_;
		allocator allocator_;
	};

}