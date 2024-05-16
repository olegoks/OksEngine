cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(LuaStatic STATIC IMPORTED)
set_property(TARGET LuaStatic PROPERTY
             IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/Lua54.lib)

add_library(Lua STATIC)   

file(GLOB_RECURSE SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.cpp
    *.hpp
    *.h
    *.c
)

target_sources(Lua
    PUBLIC
        ${SOURCES}
    )
        
source_group("Sources"
    FILES
    ${SOURCES})

target_include_directories(Lua
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/Include/)

# target_link_directories(Lua
#     INTERFACE
#         ${CMAKE_CURRENT_LIST_DIR}
#     )   

# target_link_libraries(Lua
#     INTERFACE
#         ${CMAKE_CURRENT_LIST_DIR}/Lua54.lib)

set_target_properties(Lua PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(Lua
    PUBLIC
        LuaStatic)
