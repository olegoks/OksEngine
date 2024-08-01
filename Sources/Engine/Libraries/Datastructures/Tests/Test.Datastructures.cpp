#include <gtest/gtest.h>

#include <thread>

#include <DataStructures.hpp>
#include <Datastructures.VersionedMap.hpp>


// Demonstrate some basic assertions.
TEST(TestVersionedMap, Test1) {
  
	DS::VersionedMap<int> ds;

}

TEST(TestVersionedMap, Test2) {

	DS::VersionedMap<int> ds;
	auto id1 = ds.AddElement(10);

}

TEST(TestVersionedMap, Test3) {

	DS::VersionedMap<int> ds;
	auto id1 = ds.AddElement(10);

}

TEST(TestVersionedMap, Test4) {

	DS::VersionedMap<int> ds;
	auto id1 = ds.AddElement(10);

}

TEST(TestVersionedMap, Test5) {

	DS::TS::Queue<int> queue;
	using namespace std::chrono_literals;
	std::thread thread1{ [&](){

		queue.Push(10);
		std::this_thread::sleep_for(500ms);
		queue.Push(20);
		std::this_thread::sleep_for(1s);
		queue.Push(10);
	} };

	std::this_thread::sleep_for(500ms);
	int first = 0;
	queue.WaitAndPop(first);
	
	int second = 0;
	queue.WaitAndPop(second);

	int third = 0;
	queue.WaitAndPop(third);

	thread1.join();
}



