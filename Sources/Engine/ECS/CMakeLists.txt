cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ECS STATIC)

target_sources(ECS
    PUBLIC
        ECS.Common.hpp
        ECS.Component.hpp
        ECS.ComponentsManager.hpp
        ECS.EntitiesManager.hpp
        ECS.Entity.hpp
        ECS.hpp
        ECS.System.hpp
        ECS.SystemsManager.hpp
        ECS.World.hpp
    PRIVATE
        ECS.Entity.cpp
    )

target_link_libraries(ECS
    PRIVATE
        Common)
