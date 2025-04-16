cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Config STATIC)   

file(GLOB_RECURSE SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.cpp
    *.hpp
    *.h
    *.c
)

target_sources(Config
    PUBLIC
        ${SOURCES}
    )
        
source_group("Sources"
    FILES
    ${SOURCES})

target_include_directories(Config
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Sources/)

target_link_libraries(Config 
    PUBLIC 
        LuaLayer
        )

