cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ECS STATIC)

file(GLOB PUBLIC_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.hpp
    *.h
)

file(GLOB PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.cpp
    *.c
)

target_sources(ECS
    PUBLIC
        ${PUBLIC_SOURCES}
        # ECS.Common.hpp
        # ECS.Component.hpp
        # ECS.ComponentsManager.hpp
        # ECS.EntitiesManager.hpp
        # ECS.Entity.hpp
        # ECS.hpp
        # ECS.System.hpp
        # ECS.SystemsManager.hpp
        # ECS.World.hpp
    PRIVATE
        ${PRIVATE_SOURCES}
        # ECS.Entity.cpp
    )
        
source_group("Sources"
    FILES
    ${PUBLIC_SOURCES}
    ${PRIVATE_SOURCES})


target_link_libraries(ECS
    PUBLIC
        OS)

target_include_directories(ECS
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(Tests)