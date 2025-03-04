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

include(CPack)

set(CPACK_PACKAGE_NAME "OksEngineGame") 
set(CPACK_PACKAGE_VERSION "1.0")
set(CPACK_PACKAGE_DESCRIPTION "A brief description of my software")
set(CPACK_INSTALL_PREFIX "C:/ProgramFiles (x86)/") 

# Установка исполняемого файла
install(TARGETS OksEngineGame DESTINATION bin)
#install(FILES ${CMAKE_CURRENT_LIST_DIR}/../Build/Sources/Relese DESTINATION bin)

set(CPACK_GENERATOR "ZIP")

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


