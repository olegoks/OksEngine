cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestRender Test.Render.cpp)

target_link_libraries(TestRender
  Render
  GTest::gtest_main
  )

add_test(
  NAME
    TestRender
  COMMAND
    $<TARGET_FILE:TestRender>
  )
