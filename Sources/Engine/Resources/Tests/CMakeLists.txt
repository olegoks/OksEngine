cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestResources Test.Resources.cpp)

target_link_libraries(TestResources
  Resources
  GTest::gtest_main
  )

add_test(
  NAME
    TestResources
  COMMAND
    $<TARGET_FILE:TestResources>
  )

