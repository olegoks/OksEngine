#include <gtest/gtest.h>

#include <any>
#include <Common.hpp>
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
	auto testPath = currentPath / "../../../../../Sources/Engine/Resources/Tests/";

	Resources::ResourceSystem::CreateInfo rsci{

	};

	Resources::ResourceSystem resourceSystem{ rsci };
	resourceSystem.AddResource(
		"VertexShader",
		testPath / "TestResources/Shaders/triangle.vert",
		"Root");
	resourceSystem.AddResource(
		"FragmentShader",
		testPath / "TestResources/Shaders/triangle.frag",
		"Root"
	);
	resourceSystem.LoadResource("Root/VertexShader");
	resourceSystem.LoadResource("Root/FragmentShader");
}

TEST(TestResources, LoadRootResource) {

	auto currentPath = std::filesystem::current_path();
	auto testPath = currentPath / "../../../../../Sources/Engine/Resources/Tests/";

	Resources::ResourceSystem::CreateInfo rsci{

	};

	Resources::ResourceSystem resourceSystem{ rsci };
	resourceSystem.AddResource(
		"VertexShader",
		testPath / "TestResources/Shaders/triangle.vert",
		"Root");
	resourceSystem.AddResource(
		"FragmentShader",
		testPath / "TestResources/Shaders/triangle.frag",
		"Root"
	);
	resourceSystem.LoadResource("Root");

}

TEST(TestResources, LoadAllResources)
{

	auto currentPath = std::filesystem::current_path();
	auto testPath = currentPath / "../../../../../Sources/Engine/Resources/Tests/";

	Common::UInt64 allocatedMemory = 0;
	Memory::AllocationCallbacks allocation_callbacks;
	{
		Memory::AllocationFunction allocation = 
			[](
				void* userData,
				Common::Size size,
				Common::Size alignment)->void*
			{
				return std::malloc(size);
			};
		allocation_callbacks.allocationCallback_ = allocation;
		allocation_callbacks.userData_ = &allocatedMemory;
	}

	Resources::ResourceSystem::CreateInfo rsci{
		
	};

	Resources::ResourceSystem resourceSystem{ rsci, allocation_callbacks };
	for (const auto& entry : std::filesystem::recursive_directory_iterator(testPath / "TestResources")) {
		if (std::filesystem::is_regular_file(entry)) {
			resourceSystem.AddResource(entry.path().filename().string(), entry.path(), "Root");
		}
	}

	resourceSystem.LoadAllResources();
	OS::LogInfo("tests", { "Amount of spent memory %d bytes.", allocatedMemory });
}


