#include <gtest/gtest.h>

#include <Resource.hpp>

// Demonstrate some basic assertions.
TEST(TestResources, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}


TEST(TestResources, CreateResourceSystem) {
	

	auto currentPath = std::filesystem::current_path();
	auto testPath = currentPath / "../../../../Sources/Engine/Resources/Tests/";

	Resource::ResourceSystem resourceSystem;

	

	//resourceSystem.AddResource()

}


