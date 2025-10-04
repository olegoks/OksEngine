cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(OS STATIC)
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

target_sources(OS
    PUBLIC
        ${PUBLIC_SOURCES}
    PRIVATE
        ${PRIVATE_SOURCES}
    )

set_target_properties(OS PROPERTIES LINKER_LANGUAGE CXX)

        
source_group("Sources"
FILES
${PUBLIC_SOURCES}
${PRIVATE_SOURCES})

 # Or use the header-only version
 find_package(spdlog CONFIG REQUIRED)
 target_link_libraries(OS PRIVATE spdlog::spdlog_header_only)


#find_package(loguru CONFIG REQUIRED)

target_include_directories(OS
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/${SourcesDir})
    
target_link_libraries(OS
    PUBLIC
        #loguru::loguru
        Common
        Datastructures)