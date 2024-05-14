cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(OS STATIC)

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

target_sources(OS
    PUBLIC
        ${PUBLIC_SOURCES}
        # OS.Assert.hpp
        # OS.Clock.hpp
        # OS.Exception.hpp
        # OS.FileSystem.BinaryFile.hpp
        # OS.FileSystem.File.hpp
        # OS.FileSystem.hpp
        # OS.FileSystem.TextFile.hpp
        # OS.hpp
        # OS.Logger.hpp
        # OS.Profiler.hpp
    PRIVATE
        ${PRIVATE_SOURCES}
        # OS.Exception.cpp
    )

set_target_properties(OS PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(OS
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})
    
target_link_libraries(OS
    PUBLIC
        Common)