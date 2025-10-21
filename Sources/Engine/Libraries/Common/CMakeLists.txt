cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(Common STATIC)

file(GLOB PUBLIC_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.hpp
    *.h
)

file(GLOB PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.cpp
    *.c
)

find_package(winpixevent CONFIG REQUIRED)

target_sources(Common
PUBLIC
    ${PUBLIC_SOURCES}
PRIVATE
    ${PRIVATE_SOURCES}
    )

        
    source_group("Sources"
    FILES
    ${PUBLIC_SOURCES}
    ${PRIVATE_SOURCES})

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(Common PUBLIC 
Microsoft::WinPixEventRuntime
magic_enum::magic_enum)


target_include_directories(Common
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
