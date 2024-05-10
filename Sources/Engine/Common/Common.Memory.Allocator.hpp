#pragma once

#include <memory>
#include <algorithm>
#include <vector>
#include <utility>
#include <functional>

#include <Common.hpp>
#include <OS.Assert.hpp>

namespace Common::Memory {

	//template<class Type>
	//class LinearAllocator {
	//public:

	//	using value_type = Type;
	//	using pointer = Type*;
	//	using const_pointer = const Type*;
	//	using reference = Type&;
	//	using const_reference = const Type&;
	//	using size_type = Common::Size;

	//	LinearAllocator() {
	//		memoryRaw_ = std::make_unique<Common::Byte[]>(startSizeInBytes_);
	//	}

	//	pointer allocate(size_type objectsNumber) {

	//	}

	//	template<class... Args>
	//	void construct(pointer p, Args&&... args) {

	//	}

	//	template<typename U>
	//	void destroy(pointer p) {

	//	}


	//	void deallocate(pointer pointer, size_type objectsNumber) {

	//	}

	//private:

	//	[[nodiscard]]
	//	constexpr static size_type ObjectSize() noexcept { return sizeof(Type); }

	//	static inline constexpr size_type startSize_ = 100;
	//	static inline constexpr size_type startSizeInBytes_ = ObjectSize() * startSize_;


	//	std::shared_ptr<Common::Byte[]> memory_ = nullptr;
	//	size_type size_ = 0;
	//	size_type allocated_ = 0;
	//};

	template<class Type>
	class PoolAllocator {
	public:

		using value_type = Type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = Common::Size;

		using BlockIndex = Common::Index;

		template<class U>
		constexpr PoolAllocator(const PoolAllocator <U>&) noexcept {
		
		
		}

		PoolAllocator() {
			memoryRaw_ = std::make_shared<Common::Byte[]>(ObjectSize() * startSize_, 0);
			memoryObjects_ = std::reinterpret_pointer_cast<Type[]>(memoryRaw_);
			size_ = startSize_;
			freeBlocks_.resize(size_, true);
		}

		[[nodiscard]]
		pointer allocate(size_type objectsNumber) {

			OS::AssertMessage(objectsNumber != 0, "Attempt to allocate zero objects.");
			OS::AssertMessage(objectsNumber == 1, "Allocate multiple objects is not suported.");

			using namespace Common;

			const Common::Index freeBlockIndex = FindFreeBlock();
			MarkBlockAsUsed(freeBlockIndex);
			pointer objectPointer = memoryObjects_.get() + freeBlockIndex;

			return objectPointer;
		}

		bool operator==(const PoolAllocator& other) const {
			return (memoryRaw_ == other.memoryRaw_);
		}

		bool operator!=(const PoolAllocator& other) const {
			return (memoryRaw_ != other.memoryRaw_);
		}

		template<class ...Args>
		void construct(pointer p, Args&&... args) {
			new (p) value_type(std::forward<Args>(args)...);
		}

		void destroy(pointer p) {
			p->~value_type();
		}


		void deallocate(pointer pointer, size_type objectsNumber = 1) noexcept {
			OS::AssertMessage(objectsNumber != 0, "Attempt to deallocate zero objects.");
			OS::AssertMessage(objectsNumber == 1, "deallocate multiple objects is not suported.");

			const BlockIndex blockIndex = GetBlockIndex(pointer);
			MarkBlockAsFree(blockIndex);
		}

	private:

		[[nodiscard]]
		constexpr static size_type ObjectSize() noexcept { return sizeof(Type); }

		static inline constexpr size_type startSize_ = 100;

	private:
		std::vector<bool> freeBlocks_;

		[[nodiscard]]
		BlockIndex FindFreeBlock() const noexcept {

			OS::AssertMessage(freeBlocks_.size() == size_, "");

			for (BlockIndex blockIndex = 0; blockIndex < size_; blockIndex++) {
				if (IsBlockFree(blockIndex)) {
					return blockIndex;
				}
			}

			OS::AssertFailMessage("There is not free block index.");
			return Common::Limits<BlockIndex>::Max();
		}

		BlockIndex GetBlockIndex(pointer ptr) {
			OS::AssertMessage(ptr >= memoryObjects_.get(), "Pointer is incorrect.");
			OS::AssertMessage(ptr <= memoryObjects_.get() + size_, "Pointer is incorrect.");
			const Common::Size offset = ptr - memoryObjects_.get();
			return offset;
		}

		void MarkBlockAsUsed(BlockIndex blockIndex) {
			freeBlocks_[blockIndex] = false;
		}

		void MarkBlockAsFree(BlockIndex blockIndex) {
			freeBlocks_[blockIndex] = true;
		}

		[[nodiscard]]
		bool IsBlockFree(BlockIndex blockIndex) const noexcept {
			OS::AssertMessage(blockIndex < size_, "Attempt to get state of block that doesn't exist. Out of range.");
			return freeBlocks_[blockIndex];
		}
	private:
		std::shared_ptr<Common::Byte[]> memoryRaw_ = nullptr;

		std::shared_ptr<value_type[]> memoryObjects_ = nullptr;

		size_type size_ = 0;

	};

	static_assert(AllocatorType<PoolAllocator<int>>);

	//class MutablePoolAllocator { 
	//public:

	//	using pointer = void*;
	//	using const_pointer = const pointer;
	//	using size_type = Common::Size;

	//private:

	//	class [[nodiscard]] Block {

	//		using Barrier = Common::UInt64;

	//		static constexpr Barrier barrierPattern_ = 0b1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010ull;

	//		constexpr static size_type GetBarrierSize() noexcept { return sizeof(barrierPattern_); }

	//	public:

	//		Block(Common::Size dataSize) noexcept :
	//			size_{ dataSize } {
	//		
	//			SetBarriers();
	//		
	//		}

	//		template<class Type, class ...Args>
	//		Type* Construct(Args&&... args) noexcept {
	//			OS::AssertMessage(!IsFree(), "Attempt to construct object in not allocated memory.");
	//			OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid before object creation.");
	//			Type* object = Common::Memory::Construct<Type>(GetData<pointer>(), std::forward<Args&&>(args)...);
	//			OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid after object creation.");
	//			return object;
	//		}

	//		template<class Type>
	//		void Destroy() noexcept {
	//			OS::AssertMessage(!IsFree(), "Attempt to construct object in not allocated memory.");
	//			OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid before object creation.");
	//			Type* object = GetData<Type>();
	//			object->~Type();
	//			OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid after object creation.");
	//		}

	//		pointer Allocate(size_type size) noexcept {
	//			OS::AssertMessage(IsFree(), "Attempt to allocate object in not free block.");
	//			OS::AssertMessage(size <= GetDataSize(), "Attempt to allocate memory in the block that doesn't have enouth memory.");
	//			OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid after object allocation.");
	//			Use();
	//			return GetData<pointer>();
	//		}

	//		void Deallocate(void* ptr) noexcept {
	//			OS::AssertMessage(GetData() == ptr, "Attempt to use inavalid pointer to deallocate memory.");
	//			Free();
	//		}

	//		template<class Type = void>
	//		[[nodiscard]]
	//		Type* GetData() noexcept {
	//			return reinterpret_cast<Type*>(GetRawThis() + GetHeaderSize() + GetBarrierSize());
	//		}

	//		static constexpr Common::Size MinSize() noexcept {
	//			return GetHeaderSize() + 2 * GetBarrierSize();
	//		}

	//		static constexpr Common::Size CalculateSize(Common::Size dataSize) noexcept {
	//			return MinSize() + GetAlignedMemorySize(dataSize);
	//		}

	//		static Common::Size CalculateDataSize(Common::Size blockSize) noexcept {
	//			OS::AssertMessage(IsMemorySizeAligned(blockSize), "Attempt to calculate data size by memory size, but it is not aligned.");
	//			return blockSize - MinSize();
	//		}

	//		[[nodiscard]]
	//		Common::Size GetDataSize() const noexcept {
	//			const Common::Size dataSize = GetSize() - MinSize();
	//			OS::AssertMessage(IsMemorySizeAligned(dataSize), "Block stores not aligned data memory size.");
	//			return dataSize;
	//		}

	//		[[nodiscard]]
	//		Common::Size GetSize() const noexcept {
	//			OS::AssertMessage(size_ > MinSize(), "Block size is incorrect or block stores zero bytes.");
	//			return size_;
	//		}

	//		void SetNextBlock(Block* block) noexcept { next_ = block; };

	//		[[nodiscard]]
	//		Block* GetNextBlock() noexcept { return next_; }

	//		[[nodiscard]]
	//		bool IsLast() const noexcept { return (next_ == nullptr); }

	//		[[nodiscard]]
	//		bool IsFree() const noexcept { return isFree_; }

	//		void SetSize(Common::Size newBlockSize) noexcept {

	//			OS::AssertMessage(IsMemorySizeAligned(newBlockSize), "Attempt to set not aligned block size.");
	//			OS::AssertMessage(newBlockSize > MinSize(), "Attempt to resize block to size less tham min block size.");

	//			size_ = newBlockSize;
	//		}

	//		void Resize(Common::Size newBlockSize) noexcept {

	//			OS::AssertMessage(IsFree(), "Attempt to resize not free block.");
	//			OS::AssertMessage(AreBarriersValid(), "Memory barriers are not valid.");

	//			SetSize(newBlockSize);
	//			ZeroData();
	//			SetBarriers();
	//		}

	//		void ResizeData(Common::Size newDataSize) noexcept {
	//			Resize(newDataSize + Block::MinSize());
	//		}

	//		void Use() noexcept {

	//			OS::AssertMessage(IsFree(), "Attempt to use block twice.");

	//			isFree_ = false;
	//		}

	//		void Free() noexcept {

	//			OS::AssertMessage(!IsFree(), "Attempt to free block twice.");

	//			isFree_ = false;
	//		}

	//		[[nodiscard]]
	//		static constexpr Common::Size GetHeaderSize() noexcept {
	//			return sizeof(Block);
	//		}

	//		Common::Byte* GetRawThis() noexcept {
	//			return reinterpret_cast<Common::Byte*>(this);
	//		}

	//		Barrier* GetLeftBarrier() noexcept {
	//			return reinterpret_cast<Barrier*>(GetRawThis() + GetHeaderSize());
	//		}

	//		Barrier* GetRightBarrier() noexcept {
	//			return reinterpret_cast<Barrier*>(GetRawThis() + GetHeaderSize() + GetBarrierSize() + GetDataSize());
	//		}

	//		[[nodiscard]]
	//		bool AreBarriersValid() noexcept {
	//			const Barrier leftBarrier = *GetLeftBarrier();
	//			const Barrier rightBarrier = *GetRightBarrier();

	//			return (leftBarrier == barrierPattern_) && (rightBarrier == barrierPattern_);
	//		}

	//		void SetBarriers() noexcept {
	//			*GetLeftBarrier() = barrierPattern_;
	//			*GetRightBarrier() = barrierPattern_;
	//		}

	//		void ZeroData() {
	//			std::memset(GetData<void>(), 0, GetDataSize());
	//		}

	//	private:
	//		Common::Size size_ = 0;
	//		Block* next_ = nullptr;
	//		bool isFree_ = true;
	//	};

	//public:

	//	pointer allocate(size_type size) {

	//		OS::AssertMessage(size > 0, "Attempt to allocate zero bytes.");

	//		Maybe<Block*> maybeBlock = FindFreeBlock(size);
	//		OS::AssertMessage(maybeBlock.has_value(), "There are no suitable block for allocation.");

	//		Block* block = maybeBlock.value();
	//		if (block->GetDataSize() > size) {
	//			const Common::Size extraMemorySize = block->GetDataSize() - size;
	//			const bool isThereMemoryForNewBlock = (extraMemorySize > Block::MinSize());
	//			if (isThereMemoryForNewBlock) {
	//				block->ResizeData(size);
	//				Common::Byte* newBlockMemory = reinterpret_cast<Common::Byte*>(block) + block->GetSize();
	//				Block* newBlock = Common::Memory::construct<Block>(newBlockMemory, extraMemorySize);
	//				newBlock->SetNextBlock(block->GetNextBlock());
	//				block->SetNextBlock(newBlock);
	//			}
	//		}

	//		return block->Allocate(size);
	//	}

	//	template<class Type, class ...Args>
	//	Type* construct(pointer p, Args&&... args) noexcept {
	//		OS::AssertMessage(IsBlockDataPointer(p), "Attempt to construct object using incorrect pointer.");
	//		Maybe<Block*> maybeBlock = GetBlockByBlockDataPointer(p);
	//		OS::AssertMessage(maybeBlock.has_value(), "There is no block with this block data pointer.");
	//		Block* block = maybeBlock.value();
	//		return block->Construct<Type>(std::forward<Args&&>(args)...);
	//	}
	//	

	//	template<class Type>
	//	void destroy(pointer p) {
	//		OS::AssertMessage(IsBlockDataPointer(p), "Attempt to construct object using incorrect pointer.");
	//		Maybe<Block*> maybeBlock = GetBlockByBlockDataPointer(p);
	//		OS::AssertMessage(maybeBlock.has_value(), "There is no block with this block data pointer.");
	//		Block* block = maybeBlock.value();
	//		block->Destroy<Type>();
	//	}

	//	bool operator==(const MutablePoolAllocator& other) const {
	//		return (memory_ == other.memory_);
	//	}

	//	bool operator!=(const MutablePoolAllocator& other) const {
	//		return (memory_ != other.memory_);
	//	}

	//private:

	//	[[nodiscard]]
	//	Common::Maybe<Block*> FindFreeBlock(size_type size) noexcept {

	//		Maybe<Block*> maybeBlock;
	//		ForEachBlock([&](Block* block) {
	//			if (block->IsFree() && block->GetDataSize() >= size) {
	//				maybeBlock = block;
	//				return true;
	//			}
	//		return false;
	//			});

	//		return maybeBlock;
	//	}

	//	[[nodiscard]]
	//	bool IsBlockPointer(pointer maybeBlockPointer) noexcept {
	//		bool isBlockPointer = false;
	//		ForEachBlock([&](Block* block) {
	//			if (block == maybeBlockPointer) {
	//				isBlockPointer = true;
	//				return true;
	//			}
	//		return false;
	//			});
	//		return isBlockPointer;
	//	}

	//	[[nodiscard]]
	//	bool IsBlockDataPointer(pointer maybeBlockDataPointer) noexcept {
	//		bool isBlockDataPointer = false;
	//		ForEachBlock([&](Block* block) {
	//			const pointer blockDataPointer = block->GetData();
	//			if (blockDataPointer == maybeBlockDataPointer) {
	//				isBlockDataPointer = true;
	//				return true;
	//			}
	//		return false;
	//			});
	//		return isBlockDataPointer;
	//	}

	//	Common::Maybe<Block*> GetBlockByBlockDataPointer(pointer blockDataPointer) noexcept {
	//		Common::Maybe<Block*> maybeBlock;
	//		ForEachBlock([&](Block* block) {
	//			if (block->GetData<pointer>() == blockDataPointer) {
	//				maybeBlock = block;
	//				return true;
	//			}
	//		return false;
	//			});
	//		return maybeBlock;
	//	}

	//	using ProcessBlock = std::function<bool(Block*)>;

	//	void ForEachBlock(ProcessBlock&& processBlock) noexcept {
	//		Block* currentBlock = firstBlock_;
	//		while (currentBlock != nullptr) {
	//			const bool isEnd = processBlock(currentBlock);
	//			if (isEnd) {
	//				break;
	//			}
	//			currentBlock = currentBlock->GetNextBlock();
	//		}
	//	}

	//private:
	//	Block* firstBlock_ = nullptr;
	//	std::shared_ptr<Common::Byte[]> memory_ = nullptr;
	//	size_type size_ = 0;
	//};

	//static_assert(std::equality_comparable<MutablePoolAllocator>);


}