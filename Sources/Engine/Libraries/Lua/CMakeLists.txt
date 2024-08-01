cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# add_library(LuaStatic STATIC IMPORTED)
# set_property(TARGET LuaStatic PROPERTY
#              IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/Lua54.lib)

add_library(LuaLayer STATIC)   

file(GLOB_RECURSE SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.cpp
    *.hpp
    *.h
    *.c
)

target_sources(LuaLayer
    PUBLIC
        ${SOURCES}
    )
        
source_group("Sources"
    FILES
    ${SOURCES})

target_include_directories(LuaLayer
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# target_link_directories(Lua
#     INTERFACE
#         ${CMAKE_CURRENT_LIST_DIR}
#     )   

# target_link_libraries(Lua
#     INTERFACE
#         ${CMAKE_CURRENT_LIST_DIR}/Lua54.lib)



find_package(Lua REQUIRED)
target_include_directories(LuaLayer PUBLIC ${LUA_INCLUDE_DIR})
target_link_libraries(LuaLayer 
    PUBLIC 
        ${LUA_LIBRARIES}
        OS
        )

# find_path(LUABRIDGE3_INCLUDE_DIRS ")luabridge3/LuaBridge/Array.h")
# target_include_directories(LuaLayer PUBLIC ${LUABRIDGE3_INCLUDE_DIRS})

