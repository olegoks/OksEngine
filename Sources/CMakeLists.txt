cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(OksEngineGame)


set_target_properties(OksEngineGame
    PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS
        "-cfg ${CMAKE_CURRENT_LIST_DIR}/../config.lua")

file(GLOB SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.hpp
    *.h
    *.cpp
    *.c
)

#INSTALLER



set(CPACK_PACKAGE_NAME "OksEngineGame") 
set(CPACK_PACKAGE_VERSION "1.0")
set(CPACK_PACKAGE_DESCRIPTION "A brief description of my software")
set(CPACK_INSTALL_PREFIX "C:/ProgramFiles (x86)/") 
set(CPACK_GENERATOR "ZIP")
set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include (InstallRequiredSystemLibraries)
install (PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION bin)
# install(FILES ${CMAKE_CURRENT_LIST_DIR}/../Build/Sources/Debug/ DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../Build/Sources/Release/ DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../Resources DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../Sources/Scripts DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../Sources/Engine/Sources DESTINATION bin)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/../config.lua DESTINATION bin)
# Установка исполняемого файла
install(TARGETS OksEngineGame)
#set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP FALSE)
#include (InstallRequiredSystemLibraries)
# install(TARGETS OksEngineGame
#     #PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin
#     DESTINATION bin 
#     PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
#     POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
#     DIRECTORIES $<TARGET_FILE_DIR:OksEngineGame> "${CMAKE_BINARY_DIR}"
#     COMPONENT Runtime
# )
#install(FILES ${CMAKE_CURRENT_LIST_DIR}/../Build/Sources/Relese DESTINATION bin)
include(CPack)

#INSTALLER


source_group("Sources/Engine"
    FILES
    ${SOURCES})

target_sources(OksEngineGame
    PRIVATE
        ${SOURCES})


target_link_libraries(OksEngineGame
    PRIVATE
        OksEngine
    )   


add_subdirectory(Engine)

# Добавьте конфигурацию CPack


