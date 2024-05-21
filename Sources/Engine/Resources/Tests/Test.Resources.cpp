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

	Resources::ResourceSystem resourceSystem;
	resourceSystem.AddResource(
		"VertexShader",
		testPath / "TestResources/Shaders/triangle.vert",
		"Root");
	resourceSystem.AddResource(
		"FragmentShader",
		testPath / "TestResources/Shaders/triangle.frag",
		"Root"
	);
	/*resourceSystem.LoadResource("Root/VertexShader");
	resourceSystem.LoadResource("Root/FragmentShader");
	*/

	//resourceSystem.AddResource()

}


