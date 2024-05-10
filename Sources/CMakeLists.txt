cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(Engine)

add_executable(OksEngineGame
    Main.cpp
)   

target_link_libraries(OksEngineGame
    PRIVATE
        OksEngine
    )   
