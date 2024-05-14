cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(OksEngine STATIC)   

target_sources(OksEngine
    PUBLIC
        Engine.hpp
    PRIVATE
        Engine.cpp
    )

target_link_libraries(OksEngine
    PUBLIC
        ECS
    )   

    source_group("Sources"
    FILES
    Engine.hpp
    Engine.cpp)


target_include_directories(OksEngine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})


add_subdirectory(Common)
add_subdirectory(OS)
add_subdirectory(ECS)
add_subdirectory(Tests)
    
