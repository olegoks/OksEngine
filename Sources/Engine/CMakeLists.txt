cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(OksEngine STATIC)   

# file(GLOB SOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     *.hpp
#     *.h
#     *.cpp
#     *.c
# )

# file(GLOB COMPONENTS_SOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Components\*.hpp
#     Components\*.h
#     Components\*.cpp
#     Components\*.c
# )

target_sources(OksEngine
    PUBLIC
        Engine.hpp
        Components/OksEngine.Behaviour.hpp
        Components/OksEngine.Position.hpp
        Components/OksEngine.RenderableGeometry.hpp
    PRIVATE
        Engine.cpp
        Components/OksEngine.Behaviour.cpp
    )

target_link_libraries(OksEngine
    PUBLIC
        ECS
        LuaLayer
        Render
        OS
    )

source_group("Sources"
    FILES
    Components/OksEngine.Behaviour.hpp
    Components/OksEngine.Behaviour.cpp
    Components/OksEngine.Position.hpp
    Components/OksEngine.RenderableGeometry.hpp
    Engine.hpp
    Engine.cpp)


target_include_directories(OksEngine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})


add_subdirectory(Common)
add_subdirectory(OS)
add_subdirectory(ECS)
add_subdirectory(Lua)
add_subdirectory(Tests)
add_subdirectory(Render)
add_subdirectory(Math)
add_subdirectory(Geometry)
add_subdirectory(UI)
    
