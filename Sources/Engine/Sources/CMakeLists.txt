cmake_minimum_required(VERSION  3.26.4)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



add_library(OksEngine STATIC)   


if(MSVC)
  target_compile_options(OksEngine PUBLIC "/MP")
  #  target_compile_options(OksEngine PUBLIC "/MP 20")
  target_link_options(OksEngine PUBLIC "/INCREMENTAL")
endif()


#Test code

function(get_source_files DIRECTORY FILE_LIST)
    file(GLOB_RECURSE TEMP_FILES "${DIRECTORY}/*.cpp" "${DIRECTORY}/*.hpp" "${DIRECTORY}/*.ecs")
    set(${FILE_LIST} ${TEMP_FILES} PARENT_SCOPE)

endfunction()

# Функция для создания source_group для в соответствии с их местоположением относительно корневой директории
function(organize_source_files DIRECTORY FILES)
    foreach(FILE_PATH IN LISTS FILES)
        # Убираем корневую часть пути к файлу
        file(RELATIVE_PATH REL_PATH "${DIRECTORY}" ${FILE_PATH})
        # Извлекаем родительский путь
 get_filename_component(PARENT_DIR "${REL_PATH}" DIRECTORY)
        # Заменяем '/' на '\\' в пути, для правильной работы source_group
        string(REPLACE "/" "\\" GROUP_NAME "Sources/${PARENT_DIR}")
        # Создаем source_group
        source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
        #message(STATUS "${GROUP_NAME}=${${GROUP_NAME}} ${FILE_PATH}=${${FILE_PATH}} ")
    endforeach()
endfunction()

# Получаем список всех файлов
get_source_files(${CMAKE_CURRENT_LIST_DIR} PROJECT_SOURCES)

# Организуем файлы по группам
organize_source_files(${CMAKE_CURRENT_LIST_DIR} "${PROJECT_SOURCES}")

#Test code

# file(GLOB PUBLIC_SOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     *.hpp
#     *.h
# )

# file(GLOB PRIVATE_SOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     *.cpp
#     *.c
# )

# file(GLOB PUBLIC_SOURCES_COMMON
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Common/*.hpp
#     Common/*.h
# )

# file(GLOB PRIVATE_SOURCES_COMMON
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Common/*.cpp
#     Common/*.c
# )

# file(GLOB PUBLIC_SOURCES_DEBUG
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Debug/*.hpp
#     Debug/*.h
# )

# file(GLOB PRIVATE_SOURCES_DEBUG
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Debug/*.cpp
#     Debug/*.c
# )

# file(GLOB PUBLIC_SOURCES_PHYSICS
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Physics/*.hpp
#     Physics/*.h
# )

# file(GLOB PRIVATE_SOURCES_PHYSICS
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Physics/*.cpp
#     Physics/*.c
# )

# file(GLOB PUBLIC_SOURCES_RENDER
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Render/*.hpp
#     Render/*.h
# )

# file(GLOB PRIVATE_SOURCES_RENDER
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Render/*.cpp
#     Render/*.c
# )

# file(GLOB PUBLIC_SOURCES_RESOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Resources/*.hpp
#     Resources/*.h
# )

# file(GLOB PRIVATE_SOURCES_RESOURCES
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Resources/*.cpp
#     Resources/*.c
# )

# file(GLOB PUBLIC_SOURCES_BEHAVIOUR
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Behaviour/*.hpp
#     Behaviour/*.h
# )

# file(GLOB PRIVATE_SOURCES_BEHAVIOUR
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     Behaviour/*.cpp
#     Behaviour/*.c
# )

# file(GLOB PUBLIC_SOURCES_UI
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     UI/*.hpp
#     UI/*.h
# )

# file(GLOB PRIVATE_SOURCES_UI
#     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
#     LIST_DIRECTORIES false
#     UI/*.cpp
#     UI/*.c
# )

target_sources(OksEngine
    PUBLIC
        # ${PUBLIC_SOURCES}
        # ${PUBLIC_SOURCES_UI}
        # ${PUBLIC_SOURCES_COMMON}
        # ${PUBLIC_SOURCES_RENDER}
        # ${PUBLIC_SOURCES_PHYSICS}
        # ${PUBLIC_SOURCES_DEBUG}
        # ${PUBLIC_SOURCES_BEHAVIOUR}
        # ${PUBLIC_SOURCES_RESOURCES}
        ${PROJECT_SOURCES}
    PRIVATE
        # ${PRIVATE_SOURCES}
        # ${PRIVATE_SOURCES_UI}
        # ${PRIVATE_SOURCES_COMMON}
        # ${PRIVATE_SOURCES_RENDER}
        # ${PRIVATE_SOURCES_PHYSICS}
        # ${PRIVATE_SOURCES_DEBUG}
        # ${PRIVATE_SOURCES_BEHAVIOUR}
        # ${PRIVATE_SOURCES_RESOURCES}
    )


find_package(winpixevent CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(OksEngine
    PUBLIC
    Microsoft::WinPixEventRuntime
    SFML::Audio
    SFML::System
    yaml-cpp::yaml-cpp
    imgui::imgui
    Geometry
    ECS
    ECS2
    LuaLayer
    Render
    Physics
    OS
    UI
    Resources
    )

# source_group("Sources"
#     FILES
#     ${PUBLIC_SOURCES}
#     ${PRIVATE_SOURCES}
#     )
# source_group("Sources/Common"
#     FILES
#     ${PUBLIC_SOURCES_COMMON}
#     ${PRIVATE_SOURCES_COMMON}
#     )
# source_group("Sources/Physics"
#     FILES
#     ${PUBLIC_SOURCES_PHYSICS}
#     ${PRIVATE_SOURCES_PHYSICS}
#     )
# source_group("Sources/UI"
#     FILES
#     ${PUBLIC_SOURCES_UI}
#     ${PRIVATE_SOURCES_UI}
#     )
# source_group("Sources/Debug"
#     FILES
#     ${PUBLIC_SOURCES_DEBUG}
#     ${PRIVATE_SOURCES_DEBUG}
#     )
# source_group("Sources/Behaviour"
#     FILES
#     ${PUBLIC_SOURCES_BEHAVIOUR}
#     ${PRIVATE_SOURCES_BEHAVIOUR}
#     )
# source_group("Sources/Render"
#     FILES
#     ${PUBLIC_SOURCES_RENDER}
#     ${PRIVATE_SOURCES_RENDER}
#     )
# source_group("Sources/Resources"
#     FILES
#     ${PUBLIC_SOURCES_RESOURCES}
#     ${PRIVATE_SOURCES_RESOURCES}
#     )

target_include_directories(OksEngine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})
