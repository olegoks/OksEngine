cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestDatastructures Test.Datastructures.cpp)

target_link_libraries(TestDatastructures
    Datastructures
    GTest::gtest_main
  )

add_test(
  NAME
    TestDatastructures
  COMMAND
    $<TARGET_FILE:TestDatastructures>
  )
