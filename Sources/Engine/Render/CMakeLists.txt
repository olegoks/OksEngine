cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Render STATIC)
set(SourcesDir "Sources/")

# file(GLOB_RECURSE PUBLIC_SOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     ${SourcesDir}/*.hpp
#     ${SourcesDir}/*.h
# )

# file(GLOB_RECURSE
#  PRIVATE_SOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     ${SourcesDir}/*.cpp
#     ${SourcesDir}/*.c
# )

target_sources(Render
    PUBLIC
        #${PUBLIC_SOURCES}

        Sources/RAL.API.hpp
        Sources/RAL.Camera.hpp
        Sources/RAL.Common.hpp
        Sources/RAL.Driver.hpp
        Sources/RAL.Light.hpp
        Sources/RAL.WindowSurface.hpp
        Sources/RAL.WindowSurface.cpp
        Sources/Vulkan/Render.Vulkan.Common.hpp
        Sources/Vulkan/Render.Vulkan.Driver.hpp
        Sources/Vulkan/Render.Vulkan.Camera.hpp
        Sources/Vulkan/Render.Vulkan.API.hpp
        Sources/Vulkan/Render.Vulkan.Common.hpp
    PRIVATE
        Sources/RAL.cpp
        #${PRIVATE_SOURCES}
    )

source_group("Sources"
    FILES
    Sources/RAL.API.hpp
    Sources/RAL.Camera.hpp
    Sources/RAL.Common.hpp
    Sources/RAL.Driver.hpp
    Sources/RAL.Light.hpp
    Sources/RAL.WindowSurface.hpp
    Sources/RAL.WindowSurface.cpp
    Sources/Vulkan/Render.Vulkan.Common.hpp
    Sources/Vulkan/Render.Vulkan.Driver.hpp
    Sources/Vulkan/Render.Vulkan.Camera.hpp
    Sources/Vulkan/Render.Vulkan.API.hpp
    Sources/Vulkan/Render.Vulkan.Common.hpp
    Sources/RAL.cpp
    # ${PUBLIC_SOURCES}
    # ${PRIVATE_SOURCES})
    )


target_link_libraries(Render
    PUBLIC
        OS
        Geometry
        Math)

        
find_package(Vulkan REQUIRED)
target_link_libraries(Render PRIVATE Vulkan::Vulkan)

target_include_directories(Render
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Sources/)

add_subdirectory(Tests)