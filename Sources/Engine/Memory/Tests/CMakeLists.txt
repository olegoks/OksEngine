cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestMemory Test.Memory.cpp)

target_link_libraries(TestMemory
  Memory
  GTest::gtest_main
  )

add_test(
  NAME
    TestMemory
  COMMAND
    $<TARGET_FILE:TestMemory>
  )

