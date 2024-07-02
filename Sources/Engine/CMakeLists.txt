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
    #Components/*.hpp
    #Components/*.h
    #Systems/*.hpp
    #Systems/*.h

)

file(GLOB PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false

    #Components/*.cpp
    #Components/*.c
    #Systems/*.cpp
    #Systems/*.c
    *.cpp
    *.c
)


file(GLOB PUBLIC_SOURCES_COMPONENTS
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    Components/*.hpp
    Components/*.h
)

file(GLOB PRIVATE_SOURCES_COMPONENTS
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    Components/*.cpp
    Components/*.c
)

file(GLOB PUBLIC_SOURCES_SYSTEMS
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    Systems/*.hpp
    Systems/*.h
)

file(GLOB PRIVATE_SOURCES_SYSTEMS
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    Systems/*.cpp
    Systems/*.c
)

file(GLOB PUBLIC_SOURCES_SUBSYSTEMS
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    Subsystems/*.hpp
    Subsystems/*.h
)

file(GLOB PRIVATE_SOURCES_SUBSYSTEMS
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    Subsystems/*.cpp
    Subsystems/*.c
)

target_sources(OksEngine
    PUBLIC
        ${PUBLIC_SOURCES}
        ${PUBLIC_SOURCES_COMPONENTS}
        ${PUBLIC_SOURCES_SYSTEMS}
        ${PUBLIC_SOURCES_SUBSYSTEMS}
    PRIVATE
        ${PRIVATE_SOURCES}
        ${PRIVATE_SOURCES_COMPONENTS}
        ${PRIVATE_SOURCES_SYSTEMS}
        ${PRIVATE_SOURCES_SUBSYSTEMS}
    )

target_link_libraries(OksEngine
    PUBLIC
        Geometry
        ECS
        LuaLayer
        Render
        Physics
        OS
        UI
        Resources
    )

source_group("Sources"
    FILES
    ${PUBLIC_SOURCES}
    ${PRIVATE_SOURCES}
    )

source_group("Sources/Components"
    FILES
    ${PUBLIC_SOURCES_COMPONENTS}
    ${PRIVATE_SOURCES_COMPONENTS}
    )
source_group("Sources/Systems"
    FILES
    ${PUBLIC_SOURCES_SYSTEMS}
    ${PRIVATE_SOURCES_SYSTEMS}
    )

source_group("Sources/Subsystems"
    FILES
    ${PUBLIC_SOURCES_SUBSYSTEMS}
    ${PRIVATE_SOURCES_SUBSYSTEMS}
    )

target_include_directories(OksEngine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/Components
        ${CMAKE_CURRENT_LIST_DIR}/Systems)


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
add_subdirectory(Physics)