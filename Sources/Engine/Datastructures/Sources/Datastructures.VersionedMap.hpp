#pragma once

#include <vector>

#include <memory>
#include <functional>
#include <compare>

#include <Common.Identifier.hpp>
#include <Common.Types.hpp>
#include <Memory.Common.hpp>
#include <Memory.AllocationCallbacks.hpp>
#include <OS.Assert.hpp>

namespace Datastructures {

	template<class Type/*, Memory::Allocator allocator = Memory::PoolAllocator<Type>*/>
	class [[nodiscard]] VersionedMap final {

	public:

		using Id = Common::UInt64;

		VersionedMap(Memory::AllocationCallbacks allocationCallbacks = Memory::AllocationCallbacks{}) noexcept;

		void DeleteElement(Id id);
		void Clear();
		Id AddElement(Type&& args) noexcept;
		[[nodiscard]]
		bool IsExists(Id id) noexcept;
		Type& GetElement(Id id) noexcept;
		using ProcessElement = std::function<void(Id, Type&)>;
		void ForEachElement(ProcessElement&& processor) noexcept;

	private:

		class Slot;
		using SlotIndex = Common::UInt32;
		class SlotId;

		static_assert(sizeof(SlotId) == sizeof(Id));

		Slot& GetSlot(SlotId id) {
			return GetSlot(id.slotIndex_);
		}

		Slot& GetSlot(SlotIndex index) {
			return slots_[index];
		}

		SlotIndex FindFreeSlot() noexcept;

		Memory::AllocationCallbacks allocationCallbacks_;
		std::vector<Slot> slots_;
	};

	template<class Type>
	class [[nodiscard]] VersionedMap<Type>::Slot final {
	public:

		using Version = Common::UInt32;
		constexpr static inline Common::UInt32 invalidVersion_ = Common::Limits<Common::UInt32>::Max();

		Slot() noexcept :
			version_{ 0 },
			data_{ nullptr } { }

		[[nodiscard]]
		inline bool IsFree() const noexcept { return (data_ == nullptr); }

		void Free() {
			OS::AssertMessage(IsFree(),
				"Attempt to free slot that was free.");
			Memory::Destroy<Type>(data_);
			allocationCallbacks_.freeCallback(
				allocationCallbacks_.userData,
				data_);
			data_ = nullptr;
		}

		[[nodiscard]]
		inline Version GetVersion() const noexcept { return version_; }

		void IncreaseVersion() noexcept { ++version_; }

		[[nodiscard]]
		inline Type* GetData() const noexcept { return data_; }

		void SetData(Type* data) noexcept {
			OS::AssertMessage(IsFree(),
				"Attempt to set data to not free slot.");
			data_ = data;
		}

	private:
		Version version_ = 0;
		Type* data_ = nullptr;
	};


	template<class Type>
	VersionedMap<Type>::VersionedMap(Memory::AllocationCallbacks allocationCallbacks) noexcept :
		allocationCallbacks_{ allocationCallbacks } {
		Slot freeSlot;
		slots_.resize(32, freeSlot);
	}

	template<class Type>
	class [[nodiscard]] VersionedMap<Type>::SlotId {
	public:
		SlotId(Common::UInt64 value) noexcept {
			union {
				Common::UInt64 full_;
				struct {
					Common::UInt32 index_;
					Common::UInt32 version_;
				};
			} parsedValue = value;

			slotIndex_ = parsedValue.index_;
			version_ = parsedValue.version_;

		}

		SlotId(Common::UInt32 index, Common::UInt32 version) noexcept :
			slotIndex_{ index },
			version_{ version } { }

		operator Id() noexcept {
			
			union {
				Common::UInt64 full_;
				struct {
					Common::UInt32 index_;
					Common::UInt32 version_;
				};
			} parsedValue;

			parsedValue.index_ = slotIndex_;
			parsedValue.version_ = version_;

			return parsedValue.full_;
		}

	private:
		VersionedMap<Type>::SlotIndex slotIndex_;
		VersionedMap<Type>::Slot::Version version_;
	public:

		struct Hash {
			Common::UInt64 operator()(const SlotId& id) const noexcept {
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
		bool operator==(const SlotId& id) const noexcept {
			return (slotIndex_ == id.slotIndex_) && (version_ == id.version_);
		}

		//[[nodiscard]]
		//bool operator<(const Id& id) const noexcept {
		//	return true;
		//}
	};

	template<class Type>
	void VersionedMap<Type>::DeleteElement(Id id) {
		Slot& slot = GetSlot(SlotId{ id });
		OS::AssertMessage(!slot.IsFree(),
			"Attempt to get data of free slot.");
		slot.Free();
	}

	template<class Type>
	void VersionedMap<Type>::Clear() {
		for (Slot& slot : slots_) {
			if (!slot.IsFree()) {
				slot.Free();
			}
		}
	}

	template<class Type>
	VersionedMap<Type>::Id VersionedMap<Type>::AddElement(Type&& args) noexcept {

		Type* dataPointer = (Type*)allocationCallbacks_.allocationCallback(
			allocationCallbacks_.userData,
			sizeof(Type),
			8);

		Memory::Construct<Type>(dataPointer, std::forward<Type>(args)...);

		const SlotIndex slotIndex = FindFreeSlot();
		Slot& slot = GetSlot(slotIndex);
		slot.SetData(dataPointer);
		slot.IncreaseVersion();
		return SlotId{ slotIndex, slot.GetVersion() };
	}

	template<class Type>
	bool VersionedMap<Type>::IsExists(Id id) noexcept {
		const Slot& slot = GetSlot(id);
		const bool sameVersion = (slot.GetVersion() == id.version_);
		const bool thereIsData = !slot.IsFree();
		return (sameVersion && thereIsData);
	}

	template<class Type>
	Type& VersionedMap<Type>::GetElement(Id id) noexcept {
		Slot& slot = GetSlot(id);
		OS::AssertMessage(!slot.IsFree(),
			"Attempt to get data of free slot.");
		return *slot.data_;
	}

	template<class Type>
	void VersionedMap<Type>::ForEachElement(ProcessElement&& processor) noexcept {
		for (SlotIndex slotIndex = 0; slotIndex < slots_.size(); slotIndex++) {
			const Slot& slot = slots_[slotIndex];
			if (!slot.IsFree()) {
				const SlotId slotId{ slotIndex, slot.version_ };
				processor(slotId, *slot.GetData());
			}
		}
	}

	template<class Type>
	VersionedMap<Type>::SlotIndex VersionedMap<Type>::FindFreeSlot() noexcept {
		for (SlotIndex slotIndex = 0; slotIndex < slots_.size(); slotIndex++) {
			if (GetSlot(slotIndex).IsFree()) {
				return slotIndex;
			}
		}
		OS::AssertFailMessage("There are no free slots.");
		return Common::Limits<SlotIndex>::Max();
	}

}

namespace DS = Datastructures;