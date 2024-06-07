cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestRender Test.Render.cpp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(TestRender
  Render
  GTest::gtest_main
  glfw
  )

add_test(
  NAME
    TestRender
  COMMAND
    $<TARGET_FILE:TestRender>
  )
