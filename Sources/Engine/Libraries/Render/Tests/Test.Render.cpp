#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include <RAL.hpp>

// Demonstrate some basic assertions.
TEST(TestRender, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(TestRender, CreatingDriver) {
	auto api = RAL::CreateAPI();
}

