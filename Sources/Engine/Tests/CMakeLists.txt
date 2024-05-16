cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(TestEngine Test.Engine.cpp)

target_link_libraries(TestEngine
  OksEngine
  GTest::gtest_main
  )

add_test(
  NAME
    TestEngine
  COMMAND
    $<TARGET_FILE:TestEngine>
  )

