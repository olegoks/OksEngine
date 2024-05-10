
#include <memory>
#include <algorithm>
#include <vector>
#include <utility>
#include <functional>
#include <type_traits>
#include <compare>
#include <bitset>
#include <array>

#include <Common.hpp>
#include <OS.hpp>

namespace Common::Memory {

	class [[nodiscard]] AllocationStrategy {
	public:
		
		AllocationStrategy(Common::Byte* memory, Common::Size size) noexcept : 
			memory_{ memory }, size_{ size } { 
		
			OS::AssertMessage(memory != nullptr, "Attempt to use nullptr pointer with allocation strategy.");
			OS::AssertMessage(size > 0, "Attempt to use memory with size 0 to use with allocation strategy.");

		}
		
		[[nodiscard]]
		virtual Common::Byte* Allocate(Common::Size bytesNumber) noexcept = 0;
		
		[[nodiscard]]
		virtual void Deallocate(void* pointer) noexcept = 0;

		template<class Type = void>
		[[nodiscard]]
		const Type* GetMemory() const noexcept { return const_cast<AllocationStrategy*>(this)->GetMemory<Type>(); }

		template<class Type = void>
		[[nodiscard]]
		Type* GetMemory() noexcept { return reinterpret_cast<Type*>(memory_); }

	protected:

		[[nodiscard]]
		Common::Size GetSize() const noexcept { 
			OS::AssertMessage(size_ > 0, "Invalid value of memory size.");
			return size_;
		}

	private:
		Common::Byte* memory_ = nullptr;
		Common::Size size_ = 0;
	};

	template<Common::Size blockSize = 64, Common::Size blocksNumber = 64>
	class BlockStrategy : public AllocationStrategy {
	private:
		
		template<Common::Size size = blockSize>
		class [[nodiscard]] Block final {
		public:

			[[nodiscard]]
			constexpr static Common::Size GetSize() noexcept { 
				OS::AssertMessage(size > 0, "Invalid value of size.");
				return size;
			}

			[[nodiscard]]
			Common::Byte* GetData() noexcept { 
				OS::AssertMessage(data_ != nullptr, "Invalid value of data pointer.");
				return data_.data();
			}

		private:
			std::array<Common::Byte, size> data_{ 0 };
		}; 

	public:

		explicit BlockStrategy(Common::Byte* memory, Common::Size size) noexcept : 
			AllocationStrategy{ memory, size } { 
			
			OS::AssertMessage(memory != nullptr, "Attempt to use invalid memory pointer.");
			OS::AssertMessage(size > 0, "Attempt to use memory with size 0.");
			OS::AssertMessage((size % blockSize) == 0, "Memory size doesn't suit to block size.");

			blockStates_.reset();

		}

		[[nodiscard]]
		virtual Common::Byte* Allocate(Common::Size bytesNumber) noexcept override {
			OS::AssertMessage(bytesNumber <= blockSize, "Attempt to allocate more bytes than block size.");
			const Common::Maybe<Common::Index> maybeFreeBlockIndex = GetFreeBlockIndex();
			OS::AssertMessage(maybeFreeBlockIndex.has_value(), "There is no free block to allocate memory.");
			const Common::Index freeBlockIndex = maybeFreeBlockIndex.value();
			auto* freeBlockMemory = GetBlockMemory(freeBlockIndex);
			UseBlock(freeBlockIndex);
			return freeBlockMemory;
		}
		
		virtual void Deallocate(void* pointer) noexcept override {
			const Common::Index blockIndex = GetBlockIndex(pointer);
			FreeBlock(blockIndex);
		}

	private:

		[[nodiscard]]
		[[maybe_unused]]
		bool IsBlockPointer(const void* maybeBlockPointer) const noexcept {
			OS::AssertMessage(maybeBlockPointer != nullptr, "Invalid value of pointer.");
			const Block* maybeBlockPointer = reinterpret_cast<const Block*>(maybeBlockPointer);
			auto* memory = GetMemory<Block*>();
			const Common::Size offset = maybeBlockPointer - memory;

			return (offset % blockSize) == 0;
		}

		[[nodiscard]]
		Common::Maybe<Common::Index> GetFreeBlockIndex() const noexcept {
			for (Common::Index blockStateIndex = 0; blockStateIndex < blocksNumber; blockStateIndex++) {
				const bool blockIsBusy = blockStates_[blockStateIndex];
				if (!blockIsBusy) {
					return blockStateIndex;
				}
			}
			return {};
		}

		void FreeBlock(Common::Index blockIndex) noexcept {
			OS::AssertMessage(blockIndex < blocksNumber, "Attempt to use incorrect block index.");
			OS::AssertMessage(!IsBlockFree(blockIndex), "Attempt to free black that is free yet.");
			blockStates_[blockIndex] = 0;
		}

		void UseBlock(Common::Index blockIndex) noexcept {
			OS::AssertMessage(blockIndex < blocksNumber, "Attempt to use incorrect block index.");
			OS::AssertMessage(IsBlockFree(blockIndex), "Attempt to used not free block.");
			blockStates_[blockIndex] = 1;
		}

		[[nodiscard]]
		bool IsBlockFree(Common::Index blockIndex) const noexcept {
			return !blockStates_[blockIndex];
		}

		[[nodiscard]]
		Block<blockSize>* GetBlockMemory(Common::Index blockIndex) noexcept {
			Block* firstBlock = GetMemory<Block>();
			return (firstBlock + blockIndex);
		}

		[[nodiscard]]
		Common::Index GetBlockIndex(const Block<blockSize>* block) const noexcept {
			OS::AssertMessage(IsBlockPointer(block), "Attempt to deallocate pointer that is not block begin.");
			const Block* firstBlock = GetMemory<Block>();
			return (block - firstBlock);
		}

	private:
		std::bitset<blocksNumber> blockStates_ = 0;
	};

	class [[nodiscard]] PoolStrategy : public AllocationStrategy {
	private:

		class [[nodiscard]] Block final {

			class [[nodiscard]] Barrier final {
			public:

				[[nodiscard]]
				constexpr static Common::Size GetSize() noexcept {
					return sizeof(Barrier);
				}

				auto operator<=>(const Barrier& barrierCopy) const noexcept = default;

				[[nodiscard]]
				bool IsValid() const noexcept { return (value_ == GetReferenceValue()); }

			private:

				[[nodiscard]]
				static constexpr Common::UInt64 GetReferenceValue() noexcept {
					return 0b1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010'1010ull;
				}

			private:

				Common::UInt64 value_ = GetReferenceValue();
			};

		public:

			Block(Common::Size dataSize) noexcept :
				size_{ dataSize } {

				SetBarriers();

			}

			//template<class Type, class ...Args>
			//Type* Construct(Args&&... args) noexcept {
			//	OS::AssertMessage(!IsFree(), "Attempt to construct object in not allocated memory.");
			//	OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid before object creation.");
			//	Type* object = Common::Memory::Construct<Type>(GetData(), std::forward<Args&&>(args)...);
			//	OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid after object creation.");
			//	return object;
			//}

			//template<class Type>
			//void Destroy() noexcept {
			//	OS::AssertMessage(!IsFree(), "Attempt to construct object in not allocated memory.");
			//	OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid before object destroying.");
			//	Type* object = GetData<Type>();
			//	object->~Type();
			//	OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid after object destroying.");
			//}

			[[nodiscard]]
			Common::Byte* Allocate(Common::Size size) noexcept {
				OS::AssertMessage(IsFree(), "Attempt to allocate object in not free block.");
				OS::AssertMessage(size <= GetDataSize(), "Attempt to allocate memory in the block that doesn't have enouth memory.");
				OS::AssertMessage(AreBarriersValid(), "Validation barriers are not valid after object allocation.");
				Use();
				return GetData<Common::Byte>();
			}

			void Deallocate(void* ptr) noexcept {
				OS::AssertMessage(GetData() == ptr, "Attempt to deallocate memory using invalid pointer.");
				Free();
			}

			template<class Type = void>
			[[nodiscard]]
			Type* GetData() noexcept {
				return reinterpret_cast<Type*>(GetRawThis() + GetHeaderSize() + Barrier::GetSize());
			}

			static constexpr Common::Size GetMetadataSize() noexcept {
				return GetHeaderSize() + 2 * Barrier::GetSize();
			}

			static constexpr Common::Size CalculateSizeByDataSize(Common::Size dataSize) noexcept {
				return GetMetadataSize() + GetAlignedMemorySize(dataSize);
			}

			[[nodiscard]]
			bool IsEnoughtMemoryForSeparating() noexcept {
				OS::NotImplemented();
				return false;
			}

			static Common::Size CalculateDataSize(Common::Size blockSize) noexcept {
				OS::AssertMessage(IsMemorySizeAligned(blockSize), "Attempt to calculate data size by memory size, but it is not aligned.");
				return blockSize - GetMetadataSize();
			}

			[[nodiscard]]
			Common::Size GetDataSize() const noexcept {
				const Common::Size dataSize = GetSize() - GetMetadataSize();
				OS::AssertMessage(IsMemorySizeAligned(dataSize), "Block stores not aligned data memory size.");
				return dataSize;
			}

			[[nodiscard]]
			Common::Size GetSize() const noexcept {
				OS::AssertMessage(size_ > GetMetadataSize(), "Block size is incorrect or block stores zero bytes.");
				return size_;
			}

			void SetNextBlock(Block* block) noexcept { next_ = block; };

			[[nodiscard]]
			Block* GetNextBlock() noexcept { return next_; }

			[[nodiscard]]
			void SetPreviosBlock(Block* block) noexcept { previos_ = block; }

			[[nodiscard]]
			Block* GetPreviosBlock() const noexcept { return previos_; }

			[[nodiscard]]
			bool IsLast() const noexcept { return (next_ == nullptr); }

			[[nodiscard]]
			bool IsFirst() const noexcept { return (previos_ == nullptr); }

			[[nodiscard]]
			bool IsFree() const noexcept { return isFree_; }

			void SetSize(Common::Size newBlockSize) noexcept {

				OS::AssertMessage(IsMemorySizeAligned(newBlockSize), "Attempt to set not aligned block size.");
				OS::AssertMessage(newBlockSize > GetMetadataSize(), "Attempt to resize block to size less tham min block size.");

				size_ = newBlockSize;
			}

			void Resize(Common::Size newBlockSize) noexcept {

				OS::AssertMessage(IsFree(), "Attempt to resize not free block.");
				OS::AssertMessage(AreBarriersValid(), "Memory barriers are not valid.");

				SetSize(newBlockSize);
				ZeroData();
				SetBarriers();
			}

			void ResizeData(Common::Size newDataSize) noexcept {
				Resize(newDataSize + Block::GetMetadataSize());
			}

			void Use() noexcept {

				OS::AssertMessage(IsFree(), "Attempt to use block twice.");

				isFree_ = false;
			}

			void Free() noexcept {

				OS::AssertMessage(!IsFree(), "Attempt to free block twice.");

				isFree_ = false;
			}

			[[nodiscard]]
			static constexpr Common::Size GetHeaderSize() noexcept {
				return sizeof(Block);
			}

			Common::Byte* GetRawThis() noexcept {
				return reinterpret_cast<Common::Byte*>(this);
			}

			[[nodiscard]]
			Barrier* GetLeftBarrier() noexcept {
				return reinterpret_cast<Barrier*>(GetRawThis() + GetHeaderSize());
			}

			[[nodiscard]]
			Barrier* GetRightBarrier() noexcept {
				return reinterpret_cast<Barrier*>(GetRawThis() + GetHeaderSize() + Barrier::GetSize() + GetDataSize());
			}

			[[nodiscard]]
			bool AreBarriersValid() noexcept {
				const Barrier& leftBarrier = *GetLeftBarrier();
				const Barrier& rightBarrier = *GetRightBarrier();

				return leftBarrier.IsValid() && rightBarrier.IsValid();
			}

			void SetBarriers() noexcept {
				Common::Memory::Construct<Barrier>(GetLeftBarrier());
				Common::Memory::Construct<Barrier>(GetRightBarrier());
			}

			void ZeroData() {
				std::memset(GetData<void>(), 0, GetDataSize());
			}

		private:
			Common::Size size_ = 0;
			Block* next_ = nullptr;
			Block* previos_ = nullptr;
			bool isFree_ = true;
		};
	
	public:

		PoolStrategy(Common::Byte* memory, Common::Size bytesNumber) noexcept :
			AllocationStrategy{ memory, bytesNumber } {

			OS::AssertMessage(bytesNumber > Block::GetMetadataSize(), "Attempt to allocate block in memory that doesn't have enough size.");

			Block* block = Memory::Construct<Block>(GetMemory(), bytesNumber);
			firstBlock_ = block;
		}

		[[nodiscard]]
		virtual Common::Byte* Allocate(Common::Size size) noexcept override {

			const Common::Size alignedSize = GetAlignedMemorySize(size);

			OS::AssertMessage(size > 0, "Attempt to allocate zero bytes.");

			Maybe<Block*> maybeBlock = FindFreeBlock(alignedSize);
			OS::AssertMessage(maybeBlock.has_value(), "There are no suitable block for allocation.");

			auto separateBlock = [](Block* block, Common::Size firstBlockSize, Common::Size secondBlockSize) noexcept -> Block* {
				OS::AssertMessage(firstBlockSize > Block::GetMetadataSize(), 
					"Attempt to resize block to not valid size.");
				OS::AssertMessage(secondBlockSize > Block::GetMetadataSize(),
					"Attempt to reisze block to not valid size.");
				OS::AssertMessage(block->GetSize() == firstBlockSize + secondBlockSize,
					"Incorrect sizes to separate block.");
				OS::AssertMessage(IsMemorySizeAligned(firstBlockSize) && IsMemorySizeAligned(secondBlockSize),
					"Attempt to separate block to parts with sizes that are not aligned.");
				block->Resize(firstBlockSize);
				OS::AssertMessage(block->GetSize() == firstBlockSize,
					"Size of resized block is not correct.");
				Common::Byte* secondBlockMemory = reinterpret_cast<Common::Byte*>(block) + block->GetSize();
				Block* secondBlock = reinterpret_cast<Block*>(secondBlockMemory);
				Block* createdSecondBlock = Memory::Construct<Block>(secondBlock, secondBlockSize);
				OS::AssertMessage(createdSecondBlock->GetSize() == secondBlockSize,
					"Size of resized block is not valid.");

				return createdSecondBlock;
				};

			Block* block = maybeBlock.value();
			OS::AssertMessage(IsMemorySizeAligned(block->GetSize()), "Block size is not aligned.");
			OS::AssertMessage(IsMemorySizeAligned(block->GetDataSize()), "Block data size is not aligned.");
			const bool isThereEnoughMemoryForBlockSeparating = 
				(block->GetDataSize() >= alignedSize) &&
				(block->GetDataSize() - alignedSize > Block::GetMetadataSize());
			if (isThereEnoughMemoryForBlockSeparating){
				const Common::Size extraMemorySize = block->GetDataSize() - GetAlignedMemorySize(size);
				const bool isThereMemoryForNewBlock = (extraMemorySize > Block::GetMetadataSize());
				if (isThereMemoryForNewBlock) {
					/*block->ResizeData(GetAlignedMemorySize(size));
					Common::Byte* newBlockMemory = reinterpret_cast<Common::Byte*>(block) + block->GetSize();
					Block* newBlock = Common::Memory::Construct<Block>(newBlockMemory, extraMemorySize);*/
					Block* newBlock = separateBlock(block, Block::CalculateSizeByDataSize(alignedSize), extraMemorySize);
					newBlock->SetNextBlock(block->GetNextBlock());
					newBlock->SetPreviosBlock(block);
					block->SetNextBlock(newBlock);
				}
			}

			return block->Allocate(size);
		}

		void Deallocate(void* pointer) noexcept {
			Maybe<Block*> maybeBlock = GetBlockByBlockDataPointer(pointer);
			OS::AssertMessage(maybeBlock.has_value(), "There is no block with this block data pointer.");
			Block* block = maybeBlock.value();
			block->Deallocate(pointer);
		}



		//template<class Type, class ...Args>
		//Type* Construct(void* p, Args&&... args) noexcept {
		//	OS::AssertMessage(IsBlockDataPointer(p), "Attempt to construct object using incorrect pointer.");
		//	Maybe<Block*> maybeBlock = GetBlockByBlockDataPointer(p);
		//	OS::AssertMessage(maybeBlock.has_value(), "There is no block with this block data pointer.");
		//	Block* block = maybeBlock.value();
		//	return block->Construct<Type>(std::forward<Args&&>(args)...);
		//}


		//template<class Type>
		//void destroy(void* p) {
		//	OS::AssertMessage(IsBlockDataPointer(p), "Attempt to construct object using incorrect pointer.");
		//	Maybe<Block*> maybeBlock = GetBlockByBlockDataPointer(p);
		//	OS::AssertMessage(maybeBlock.has_value(), "There is no block with this block data pointer.");
		//	Block* block = maybeBlock.value();
		//	block->Destroy<Type>();
		//}

	private:

		[[nodiscard]]
		Common::Maybe<Block*> FindFreeBlock(Common::Size size) noexcept {

			Maybe<Block*> maybeBlock;
			ForEachBlock([&](Block* block) {
				if (block->IsFree() && block->GetDataSize() >= size) {
					maybeBlock = block;
					return true;
				}
				return false;
				});

			return maybeBlock;
		}

		[[nodiscard]]
		bool IsBlockPointer(void* maybeBlockPointer) noexcept {
			bool isBlockPointer = false;
			ForEachBlock([&](Block* block) {
				if (block == maybeBlockPointer) {
					isBlockPointer = true;
					return true;
				}
				return false;
				});
			return isBlockPointer;
		}

		[[nodiscard]]
		bool IsBlockDataPointer(void* maybeBlockDataPointer) noexcept {
			bool isBlockDataPointer = false;
			ForEachBlock([&](Block* block) {
				const void* blockDataPointer = block->GetData();
				if (blockDataPointer == maybeBlockDataPointer) {
					isBlockDataPointer = true;
					return true;
				}
				return false;
				});
			return isBlockDataPointer;
		}

		Common::Maybe<Block*> GetBlockByBlockDataPointer(void* blockDataPointer) noexcept {
			Common::Maybe<Block*> maybeBlock;
			ForEachBlock([&](Block* block) {
				if (block->GetData<void*>() == blockDataPointer) {
					maybeBlock = block;
					return true;
				}
				return false;
				});
			return maybeBlock;
		}

		using ProcessBlock = std::function<bool(Block*)>;

		void ForEachBlock(ProcessBlock&& processBlock) noexcept {
			Block* currentBlock = firstBlock_;
			while (currentBlock != nullptr) {
				const bool isEnd = processBlock(currentBlock);
				if (isEnd) {
					break;
				}
				currentBlock = currentBlock->GetNextBlock();
			}
		}

	private:
		Block* firstBlock_ = nullptr;
	};

	template<class AllocationStrategyType>
	class [[nodiscard]] Allocator {
	public:

		explicit Allocator(Common::Byte* memory, Common::Size bytesNumber) noexcept :
			strategy_{ memory, bytesNumber } {

		}

		template<class Type>
		[[nodiscard]]
		Common::Byte* Allocate() noexcept {
			return reinterpret_cast<Common::Byte*>(strategy_.Allocate(sizeof(Type)));
		}

		[[nodiscard]]
		Common::Byte* Allocate(Common::Size bytesNumber) noexcept {
			return reinterpret_cast<Common::Byte*>(strategy_.Allocate(bytesNumber));
		}

		void Deallocate(void* memoryPointer) noexcept {
			strategy_.Deallocate(memoryPointer);
		}

		template<class Type, class ...Args>
		Type* Construct(void* memory, Args&& ... args) noexcept {
			return new(memory) Type{ std::forward<Args&&>(args)...};
		}

		template<class Type, class ... Args>
		[[nodiscard]]
		Type* Create(Args&& ... args) noexcept {
			void* allocatedMemory = Allocate<Type>();
			Type* constructedObject = Construct<Type>(allocatedMemory, std::forward<Args&&>(args)...);
			return constructedObject;
		}
	
	protected:

		template<class Type = void>
		[[nodiscard]]
		Type* GetMemory() noexcept { return strategy_.GetMemory<Type>(); }

	private:
		AllocationStrategyType strategy_ = nullptr;
	};

	class DynamicPoolAllocator : public Allocator<PoolStrategy> {
	public:

		DynamicPoolAllocator(Common::Size bytesNumber) noexcept :
			Allocator{ new Common::Byte[bytesNumber], bytesNumber } { }

		~DynamicPoolAllocator() noexcept {
			delete GetMemory();
		}
	};


	namespace StdCompatible {

		template<class Type>
		class DynamicPoolAllocator final {
		public:

			using pointer = Type*;
			using const_pointer = const pointer;
			using void_pointer = void*;
			using const_void_pointer = const void_pointer;
			using value_type = Type;
			using size_type = Common::Size;
			using difference_type = signed int;
		
		public:

			template<class AnotherType>
			DynamicPoolAllocator(const DynamicPoolAllocator<AnotherType>& copyAllocator) noexcept :
				poolAllocator_{ copyAllocator.poolAllocator_ } { }

			DynamicPoolAllocator() noexcept :
				poolAllocator_{ std::make_shared<Memory::DynamicPoolAllocator>(1_MB) } { }

			DynamicPoolAllocator(Common::Size size) noexcept : 
				poolAllocator_{ std::make_shared<Memory::DynamicPoolAllocator>(size) } { }

			DynamicPoolAllocator(const DynamicPoolAllocator& copyAllocator) noexcept :
				poolAllocator_{ copyAllocator.poolAllocator_ } { }

			DynamicPoolAllocator& operator=(const DynamicPoolAllocator& copyAllocator) noexcept {
				poolAllocator_ = copyAllocator.poolAllocator;
			}

			[[nodiscard]]
			bool operator==(const DynamicPoolAllocator& copyAllocator) const noexcept {
				return (poolAllocator_ == copyAllocator.poolAllocator_);
			}

			[[nodiscard]]
			bool operator!=(const DynamicPoolAllocator& copyAllocator) const noexcept {
				return !(*this == copyAllocator);
			}

			template<class ...Args>
			void construct(pointer p, Args&&... args) {
				new (p) value_type(std::forward<Args&&>(args)...);
			}

			void destroy(pointer p) {
				p->~value_type();
			}

		public:

			[[nodiscard]]
			pointer allocate(size_type objectsNumber = 1)noexcept {
				OS::AssertMessage(objectsNumber != 0, "Attempt to allocate zero objects.");
				return (pointer)poolAllocator_->Allocate(sizeof(Type) * objectsNumber);
			}

			void deallocate(pointer pointer, size_type objectsNumber = 1) noexcept {
				OS::AssertMessage(objectsNumber != 0, "Attempt to deallocate zero objects.");
				poolAllocator_->Deallocate(pointer);
			}

		public:
			std::shared_ptr<Memory::DynamicPoolAllocator> poolAllocator_ = nullptr;
		};

	}


	//template<class Type>
	//class BlockAllocator {
	//private:

	//	[[nodiscard]]
	//	inline constexpr static Common::Size GetObjectSize() noexcept { return sizeof(Type); }

	//	
	//	class [[nodiscard]] Block final {
	//	private:
	//		constexpr static Common::Size size_ = (GetObjectSize() > pointerSize) ? (GetObjectSize()) : (pointerSize);
	//	public:

	//		[[nodiscard]]
	//		constexpr Common::Size GetSize() const noexcept { return size_; }

	//		[[nodiscard]]
	//		Block* GetNext() noexcept { return next_; }

	//		void SetNext(Block* block) noexcept { next_ = block; }

	//		template<class Type>
	//		[[nodiscard]]
	//		Type* GetData() noexcept { return reinterpret_cast<Type*>(data_); }

	//	private:
	//		union {
	//			Common::Byte data_[size_] = { 0 };
	//			Block* next_;
	//		};
	//	};

	//	void InitializeBlocksList(Block* blocks, Common::Size blocksNumber) noexcept {
	//		for (Common::Index blockIndex = 0; blockIndex < blocksNumber - 1; blockIndex++) {
	//			Block* currentBlock = GetBlock(blockIndex);
	//			Block* nextBlock = GetBlock(blockIndex + 1);
	//			currentBlock->SetNext(nextBlock);
	//		}
	//		Block* lastBlock = GetBlock(blocksNumber - 1);
	//		lastBlock->SetNext(nullptr);
	//	}

	//public:

	//	[[nodiscard]]
	//	constexpr static Common::Size GetMemorySizeForObjects(Common::Size objectsNumber) noexcept {
	//		return objectsNumber * Block::GetSize();
	//	}

	//	BlockAllocator(Common::Byte* memory, Common::Size bytesNumber) noexcept {
	//		const Common::Size objectsNumber = bytesNumber / Block::GetSize();
	//		OS::AssertMessage(bytesNumber % Block::GetSize() == 0, "Bytes number doesn't fit to block size.");
	//		memory_ = new (memory) Block[objectsNumber];
	//		size_ = objectsNumber;
	//		firstFreeBlock_ = memory_;
	//	}

	//	BlockAllocator(Common::Size maxObjectsNumber) {
	//		
	//		memory_ = new (std::nothrow) Block[maxObjectsNumber];
	//		if (memory_ == nullptr) { throw Exception{}; }
	//		size_ = maxObjectsNumber;
	//		firstFreeBlock_ = memory_;
	//		
	//		InitializeBlocksList(memory_, size_);

	//	}

	//	[[nodiscard]]
	//	void* Allocate(Common::Size objectsNumber = 1) {

	//		OS::AssertMessage(objectsNumber != 0, "Attempt to allocate zero objects.");
	//		OS::AssertMessage(objectsNumber == 1, "Allocate multiple objects is not suported.");

	//		if (firstFreeBlock_ == nullptr) {
	//			throw Exception{};
	//		}

	//		Block* allocatedBlock = firstFreeBlock_;
	//		firstFreeBlock_ = firstFreeBlock_->GetNext();

	//		return allocatedBlock;
	//	}

	//	template<class ...Args>
	//	Type* Construct(void* p, Args&&... args) {
	//		return new (p) Type(std::forward<Args>(args)...);
	//	}

	//	void Destroy(void* p) {
	//		p->~Type();
	//	}

	//	void Deallocate(void* pointer) noexcept {
	//		OS::AssertMessage((pointer - memory_) % Block::GetSize() == 0, "Incorrect block pointer for deallocation.");
	//		[[maybe_unused]]
	//		bool pointerIsFreeBlock = false;
	//		ForEachFreeBlock([&pointerIsFreeBlock](Block* block) {
	//			if (block == pointer) { pointerIsFreeBlock = true; }
	//			});
	//		OS::AssertMessage(!pointerIsFreeBlock, "Incorrect pointer to deallocate.");
	//		Block* block = reinterpret_cast<Block*>(pointer);
	//		block->SetNext(firstFreeBlock_);
	//		firstFreeBlock_ = block;
	//	}

	//	bool operator==(const BlockAllocator& other) const = delete;
	//	BlockAllocator& operator=(const BlockAllocator& copyAllocator) noexcept = delete;
	//	BlockAllocator(const BlockAllocator& copyAllocator) noexcept = delete;

	//private:

	//	Block* GetBlock(Common::Size index) noexcept {
	//		return memory_ + index;
	//	}

	//	Common::Byte* GetBlockNextBlock(Common::Byte* block) noexcept {
	//		Common::Byte** blockNextBlock = reinterpret_cast<Common::Byte**>(block);
	//	}

	//	using ProcessBlock = std::function<void(Block*)>;

	//	[[maybe_unused]]
	//	void ForEachFreeBlock(ProcessBlock&& processBlock) noexcept {
	//		Block* currentBlock = firstFreeBlock_;
	//		while (currentBlock != nullptr) {
	//			processBlock(currentBlock);
	//			currentBlock = currentBlock->GetNext();
	//		}
	//	}

	//private:
	//	Block* memory_ = nullptr;
	//	Common::Size size_ = 0;
	//	Block* firstFreeBlock_ = nullptr;
	//};

}