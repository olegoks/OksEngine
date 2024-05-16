cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(Engine)

add_executable(OksEngineGame)

file(GLOB SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.hpp
    *.h
    *.cpp
    *.c
)

source_group("Sources"
    FILES
    ${SOURCES})

target_sources(OksEngineGame
    PRIVATE
        ${SOURCES})

target_link_libraries(OksEngineGame
    PRIVATE
        OksEngine
    )   

