#include <gtest/gtest.h>

#include "Engine.hpp"

// Demonstrate some basic assertions.
TEST(TestEngine, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(TestEngine, CreatingEngine) {

	OksEngine::Engine engine;
	EXPECT_FALSE(true);
}