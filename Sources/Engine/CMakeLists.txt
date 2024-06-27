cmake_minimum_required(VERSION  3.26.4)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(OksEngine STATIC)   

file(GLOB PUBLIC_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.hpp
    *.h
    Components/*.hpp
    Components/*.h
    Systems/*.hpp
    Systems/*.h

)

file(GLOB PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false

    Components/*.cpp
    Components/*.c
    Systems/*.cpp
    Systems/*.c
    *.cpp
    *.c
)



target_sources(OksEngine
    PUBLIC
    ${PUBLIC_SOURCES}
        # Engine.hpp
        # Components/OksEngine.Behaviour.hpp
        # Components/OksEngine.Position.hpp
        # Components/OksEngine.RenderableGeometry.hpp
        # OksEngine.Render.Subsystem.hpp
        # OksEngine.Resource.Subsystem.hpp
        # OksEngine.UI.Subsystem.hpp
        # OksEngine.Subsystem.hpp
    PRIVATE
    ${PRIVATE_SOURCES}
        # Engine.cpp
        # Components/OksEngine.Behaviour.cpp
    )

target_link_libraries(OksEngine
    PUBLIC
        Geometry
        ECS
        LuaLayer
        Render
        OS
        UI
        Resources
    )

source_group("Sources"
    FILES
    ${PUBLIC_SOURCES}
     ${PRIVATE_SOURCES}
    # OksEngine.Resource.Subsystem.hpp
    # Components/OksEngine.Behaviour.hpp
    # Components/OksEngine.Behaviour.cpp
    # Components/OksEngine.Position.hpp
    # Components/OksEngine.RenderableGeometry.hpp
    # OksEngine.Render.Subsystem.hpp
    # OksEngine.UI.Subsystem.hpp
    # OksEngine.Subsystem.hpp
    # Engine.hpp
    # Engine.cpp
    )


target_include_directories(OksEngine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/Components)


add_subdirectory(Common)
add_subdirectory(OS)
add_subdirectory(ECS)
add_subdirectory(Lua)
add_subdirectory(Tests)
add_subdirectory(Render)
add_subdirectory(Math)
add_subdirectory(Geometry)
add_subdirectory(UI)
add_subdirectory(Resources)    
add_subdirectory(Datastructures)