cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(OS STATIC)
set(SourcesDir "Sources/")

file(GLOB_RECURSE PUBLIC_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/*.hpp
    ${SourcesDir}/*.h
)

file(GLOB_RECURSE
 PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/*.cpp
    ${SourcesDir}/*.c
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

        
source_group("Sources"
FILES
${PUBLIC_SOURCES}
${PRIVATE_SOURCES})

find_path(LOGURU_INCLUDE_DIRS "loguru/")

add_library(loguru
    ${LOGURU_INCLUDE_DIRS}/loguru/loguru.cpp)

target_include_directories(OS
    PUBLIC
        ${LOGURU_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}/${SourcesDir})
    
target_link_libraries(OS
    PUBLIC
        loguru
        Common
        Datastructures)