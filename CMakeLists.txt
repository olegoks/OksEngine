set(CMAKE_RULE_MESSAGES ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#include(Utils)

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SUPPRESS_REGENERATION true) # To exclude ZERO_CHECK target

set(Sources "Sources/")

project("OksEngine0.1"
    VERSION 1.0.0
    DESCRIPTION ""
    LANGUAGES CXX
)

# option(INSTALL_PATH     "Installing path"                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/")
# option(BUILD_TESTS      "Build ${PROJECT_NAME} tests"                       OFF)
# option(BUILD_EXAMPLES   "Build ${PROJECT_NAME} examples"                    OFF)
# option(BUILD_DOCS       "Build ${PROJECT_NAME} documentation"               OFF)
# option(INSTALL          "Generate target for installing ${PROJECT_NAME}"    OFF)

# if (PROJECT_IS_TOP_LEVEL)
#     message(STATUS "This project is a top-level one")
# else()
#     message(STATUS "This project has a top-level one called [${CMAKE_PROJECT_NAME}]")
# endif()

file(GLOB_RECURSE SOURCES
    RELATIVE ${CMAKE_SOURCE_DIR}
    LIST_DIRECTORIES false
    ${Sources}/*.cpp
    ${Sources}/*.hpp
)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCES}
)

source_group(TREE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    FILES  
    ${SOURCES} 
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${Sources}>
)
