cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Geometry STATIC)   

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

# file(GLOB_RECURSE SOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     *.cpp
#     *.hpp
#     *.h
#     *.c
# )

target_sources(Geometry
    PUBLIC
        ${PUBLIC_SOURCES}
    PRIVATE
        ${PRIVATE_SOURCES}
    )
        
source_group("Sources"
    FILES
    ${PUBLIC_SOURCES}
    ${PRIVATE_SOURCES})

target_include_directories(Geometry
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Sources/)

find_package(tinyobjloader CONFIG REQUIRED)

target_link_libraries(Geometry
    PUBLIC
        OS
        Math
    PRIVATE 
        tinyobjloader::tinyobjloader)


