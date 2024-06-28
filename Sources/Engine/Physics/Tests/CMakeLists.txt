cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestPhysics Test.Physics.cpp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(TestPhysics
  Physics
  GTest::gtest_main
  )

add_test(
  NAME
  TestPhysics
  COMMAND
    $<TARGET_FILE:TestPhysics>
  )
