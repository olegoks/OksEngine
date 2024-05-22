#include <gtest/gtest.h>

#include <Datastructures.Vector.hpp>

// Demonstrate some basic assertions.
TEST(TestVector, Test1) {

	struct MemoryStat {
		Common::UInt64 allocated_ = 0;
		std::vector<void*> allocatedMemory;
		Common::UInt64 released_ = 0;
		std::vector<void*> releasedMemory;
	} memStat;

	Memory::AllocationCallbacks alloc;
	{
		alloc.userData_ = &memStat;
		alloc.allocationCallback_ = [](void* userData, Common::Size size, Common::Size alignment)->void* {
			auto* memStat = static_cast<MemoryStat*>(userData);
			memStat->allocated_ += size;

			void* memory = std::malloc(size);
			memStat->allocatedMemory.push_back(memory);
			userData = memStat;
			return memory;
		};
		alloc.freeCallback_ = [](void* userData, void* memory)->void {
			auto* memStat = static_cast<MemoryStat*>(userData);
			memStat->releasedMemory.push_back(memory);
			return std::free(memory);
		};
	}

	{
		DS::Vector<int> ds{ alloc };
		ds.Resize(200);
		ds.Resize(100);
		ds.Reserve(1000);
		ds.Resize(500);
	}
	auto stat = alloc.GetUserData<MemoryStat>();
}

