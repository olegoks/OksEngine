cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Render STATIC)
set(SourcesDir "Sources/")

file(GLOB PUBLIC_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/*.hpp
    ${SourcesDir}/*.h
)

file(GLOB
 PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/*.cpp
    ${SourcesDir}/*.c
)

file(GLOB PUBLIC_SOURCES_VULKAN_DRIVER
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/Vulkan/Driver/*.hpp
    ${SourcesDir}/Vulkan/Driver/*.h
)

file(GLOB PRIVATE_SOURCES_VULKAN_DRIVER
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/Vulkan/Driver/*.cpp
    ${SourcesDir}/Vulkan/Driver/*.c
)

file(GLOB PUBLIC_SOURCES_VULKAN
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/Vulkan/*.hpp
    ${SourcesDir}/Vulkan/*.h
)

file(GLOB PRIVATE_SOURCES_VULKAN
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/Vulkan/*.cpp
    ${SourcesDir}/Vulkan/*.c
)


target_sources(Render
    PUBLIC
        ${PUBLIC_SOURCES}
    	${PUBLIC_SOURCES_VULKAN}
        ${PUBLIC_SOURCES_VULKAN_DRIVER}
    PRIVATE

        ${PRIVATE_SOURCES}
        ${PRIVATE_SOURCES_VULKAN}
        ${PRIVATE_SOURCES_VULKAN_DRIVER}
    )

source_group("Sources"
    FILES
        ${PUBLIC_SOURCES}
        ${PRIVATE_SOURCES}
)

source_group("Sources/Vulkan"
    FILES
        ${PUBLIC_SOURCES_VULKAN}
        ${PRIVATE_SOURCES_VULKAN}
)

source_group("Sources/Vulkan/Driver"
    FILES
        ${PUBLIC_SOURCES_VULKAN_DRIVER}
        ${PRIVATE_SOURCES_VULKAN_DRIVER}
)


find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(VulkanUtilityLibraries CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
      
target_link_libraries(Render
    PUBLIC
        OS
        Geometry
        Math
        glfw
    PRIVATE 
        Vulkan::Vulkan
        Vulkan::LayerSettings
        Vulkan::UtilityHeaders 
        Vulkan::CompilerConfiguration
        glslang::glslang 
        glslang::glslang-default-resource-limits 
        glslang::SPIRV 
        glslang::SPVRemapper)

        
target_include_directories(Render
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Sources/
        ${CMAKE_CURRENT_LIST_DIR}/Sources/Vulkan
        ${CMAKE_CURRENT_LIST_DIR}/Sources/Vulkan/Driver)
        
add_subdirectory(Tests)