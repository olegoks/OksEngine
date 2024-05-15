cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(TestECS Test.ECS.cpp)

target_link_libraries(TestECS
  ECS
  GTest::gtest_main
  )

add_test(
  NAME
    TestECS
  COMMAND
    $<TARGET_FILE:TestECS>
  )

