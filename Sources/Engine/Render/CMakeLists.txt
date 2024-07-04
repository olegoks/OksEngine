cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Render STATIC)
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

target_sources(Render
    PUBLIC
        ${PUBLIC_SOURCES}

        # Sources/RAL.API.hpp
        # Sources/RAL.Camera.hpp
        # Sources/RAL.Common.hpp
        # Sources/RAL.Driver.hpp
        # Sources/RAL.Light.hpp
        # Sources/RAL.WindowSurface.hpp

        # Sources/Vulkan/Render.Vulkan.Common.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.hpp
        # Sources/Vulkan/Render.Vulkan.Camera.hpp
        # Sources/Vulkan/Render.Vulkan.API.hpp
        # Sources/Vulkan/Render.Vulkan.Common.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.Instance.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.WindowSurface.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.PhysicalDevice.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.LogicDevice.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.ShaderModule.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.CommandPool.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.UniformBuffer.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.DescriptorSet.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.DescriptorPool.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.DescriptorSetLayout.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.Image.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.ImageView.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.Pipeline.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.PipelineLayout.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.FrameBuffer.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.DeviceMemory.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.Semaphore.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.Fence.hpp
        # SOurces/Vulkan/Render.Vulkan.Driver.DepthBufferFeature.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.Buffer.hpp
        # Sources/Vulkan/Render.Vulkan.Driver.StagingBuffer.hpp
    PRIVATE
        #  Sources/RAL.cpp
        #  Sources/RAL.WindowSurface.cpp
        #  Sources/Vulkan/Render.Vulkan.Common.cpp
        #  Sources/Vulkan/Render.Vulkan.Driver.Instance.cpp
        #  Sources/Vulkan/Render.Vulkan.Driver.WindowSurface.cpp

        ${PRIVATE_SOURCES}
    )

source_group("Sources"
    FILES
    # Sources/RAL.API.hpp
    # Sources/RAL.Camera.hpp
    # Sources/RAL.Common.hpp
    # Sources/RAL.Driver.hpp
    # Sources/RAL.Light.hpp
    # Sources/RAL.WindowSurface.hpp
    # Sources/RAL.WindowSurface.cpp
    # Sources/Vulkan/Render.Vulkan.Common.hpp
    # Sources/Vulkan/Render.Vulkan.Common.cpp
    # Sources/Vulkan/Render.Vulkan.Driver.hpp
    # Sources/Vulkan/Render.Vulkan.Camera.hpp
    # Sources/Vulkan/Render.Vulkan.API.hpp
    # Sources/Vulkan/Render.Vulkan.Common.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.WindowSurface.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.WindowSurface.cpp
    # Sources/Vulkan/Render.Vulkan.Driver.PhysicalDevice.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.LogicDevice.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.ShaderModule.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.CommandPool.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.UniformBuffer.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.DescriptorSet.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.DescriptorPool.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.DescriptorSetLayout.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.Image.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.ImageView.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.Pipeline.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.PipelineLayout.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.FrameBuffer.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.Buffer.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.StagingBuffer.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.DeviceMemory.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.Semaphore.hpp
    # Sources/Vulkan/Render.Vulkan.Driver.Fence.hpp
    # SOurces/Vulkan/Render.Vulkan.Driver.DepthBufferFeature.hpp
    # Sources/RAL.cpp
     ${PUBLIC_SOURCES}
     ${PRIVATE_SOURCES}
    )


find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(VulkanUtilityLibraries CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
      
find_package(imgui CONFIG REQUIRED)
target_link_libraries(Render PRIVATE imgui::imgui)

find_package(implot CONFIG REQUIRED)
target_link_libraries(Render PRIVATE implot::implot)

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
        ${CMAKE_CURRENT_LIST_DIR}/Sources/Vulkan)
        
add_subdirectory(Tests)