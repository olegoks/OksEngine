cmake_minimum_required(VERSION VERSION 3.29)

set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake") #For vcpkg

project("OksEngine0_1"
    VERSION 0.1.0
    DESCRIPTION ""
    LANGUAGES CXX
)

set(CMAKE_RULE_MESSAGES ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_SUPPRESS_REGENERATION true) # To exclude ZERO_CHECK target
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets") #To add folder in VS for cmake targets.

# option(INSTALL_PATH     "Installing path"                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/")
# option(BUILD_TESTS      "Build ${PROJECT_NAME} tests"                       OFF)
# option(BUILD_EXAMPLES   "Build ${PROJECT_NAME} examples"                    OFF)
# option(BUILD_DOCS       "Build ${PROJECT_NAME} documentation"               OFF)
# option(INSTALL          "Generate target for installing ${PROJECT_NAME}"    OFF)

#include(CMakePrintHelpers)

#set(SourcesDirectory "Sources/")
# file(GLOB_RECURSE AllSources
#         RELATIVE ${CMAKE_SOURCE_DIR} # If RELATIVE flag is specified, the results will be returned as relative paths to the given path.
#         LIST_DIRECTORIES false # By default GLOB lists directories. Directories are omitted in the result if LIST_DIRECTORIES is set to false.
#         ${SourcesDirectory}/*.cpp
#         ${SourcesDirectory}/*.hpp
#     )

# set(SourcesWithoutTestSources)
# foreach(file ${AllSources})
#     if(NOT file MATCHES "test_*")
#         list(APPEND SourcesWithoutTestSources ${file})
#     endif()
# endforeach()

# add_executable(${PROJECT_NAME})

# target_sources(${PROJECT_NAME}
#     PRIVATE
#     Main.cpp
# )

# target_link_libraries(${PROJECT_NAME}
# )

# source_group(TREE 
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     FILES  
#     ${SourcesWithoutTestSources} 
# )

# target_include_directories(${PROJECT_NAME}
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SourcesDirectory}>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SourcesDirectory}/Engine/ECS>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SourcesDirectory}/Common>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SourcesDirectory}/OS>
# )





# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )

# FetchContent_GetProperties(googletest)

# if(NOT googletest_POPULATED)
#     FetchContent_Populate(googletest)

#     set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#     set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)

#     add_subdirectory(
#         ${googletest_SOURCE_DIR}
#         ${googletest_BINARY_DIR}
#     )

#     if(MSVC)
#         foreach(_tgt gtest gtest_main gmock gmock_main)
#             target_compile_definitions(${_tgt}
#                 PRIVATE
#                     "_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING"
#                     )
#         endforeach()
#     endif()

# endif()


enable_testing()

find_package(GTest CONFIG REQUIRED)


#add_executable(TestEngine "")

# set(TestSources)

# foreach(file ${AllSources})
#     cmake_path(GET file FILENAME FILE_NAME)
#     if(${FILE_NAME} MATCHES "test_*")
#         list(APPEND TestSources ${file})
#     endif()
# endforeach()

# target_sources(TestEngine
#     PRIVATE
#         ${TestSources})
 
# target_link_libraries(TestEngine
#     PRIVATE
#         GTest::gtest_main)   

# target_include_directories(TestEngine 
#     PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/${SourcesDirectory}/Engine/ECS/
#     ${CMAKE_CURRENT_SOURCE_DIR}/${SourcesDirectory}/Common
#     ${CMAKE_CURRENT_SOURCE_DIR}/${SourcesDirectory}/OS)

        
# include(GoogleTest)
# gtest_discover_tests(TestEngine)

# add_test(
#     NAME EngineTest
#     COMMAND $<TARGET_FILE:TestEngine>)

 add_subdirectory(Sources)
