cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(UI STATIC)
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

target_sources(UI
    PUBLIC
        ${PUBLIC_SOURCES}
    PRIVATE
        ${PRIVATE_SOURCES}
    )

source_group("Sources"
    FILES
    ${PUBLIC_SOURCES}
    ${PRIVATE_SOURCES})


target_link_libraries(UI
    PUBLIC
        OS
        Math
        ImGui)

target_include_directories(UI
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Sources/)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(UI PRIVATE glfw)
      
# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(UI PRIVATE imgui::imgui)
# find_package(implot CONFIG REQUIRED)
# target_link_libraries(UI PRIVATE implot::implot)

#add_subdirectory(Tests)