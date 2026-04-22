cmake_minimum_required(VERSION 3.26.4)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



add_library(OksEngine STATIC)
if(MSVC)
    #target_compile_options(OksEngine PUBLIC "/MP" "/bigobj")
    string(TIMESTAMP CURRENT_HOUR "%H")
    if(FALSE)
        #if(CURRENT_HOUR GREATER_EQUAL "00" AND CURRENT_HOUR LESS "03")
        target_compile_options(OksEngine PUBLIC "/bigobj")
        message(STATUS "Ночной режим активирован (00:00–03:00)")
    else()
        target_compile_options(OksEngine PUBLIC "/MP" "/bigobj")
        message(STATUS "Дневной режим (03:00–23:59)")
    endif()

    target_compile_options(OksEngine PUBLIC "/MP 8")
    target_link_options(OksEngine PUBLIC "/INCREMENTAL")
endif()
#TODO: Move this definition to code.
target_compile_definitions(OksEngine PUBLIC USE_PIX)
target_compile_definitions(OksEngine PUBLIC WIN32_LEAN_AND_MEAN )
# target_compile_options(OksEngine INTERFACE
#     $<$<CONFIG:Release>:
#         -Ot
#         -DNDEBUG
#     >
# )

#Test code

function(get_source_files DIRECTORY FILE_LIST)
    file(GLOB_RECURSE TEMP_FILES "${DIRECTORY}/*.glsl" "${DIRECTORY}/*.cpp" "${DIRECTORY}/*.c" "${DIRECTORY}/*.hpp" "${DIRECTORY}/*.h" "${DIRECTORY}/*.inl" "${DIRECTORY}/*.ecs")
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
#get_source_files("${CMAKE_SOURCE_DIR}/Shared/Sources" PROJECT_SHARED_SOURCES)

#message(STATUS "${PROJECT_SHARED_SOURCES}")
#organize_source_files("${CMAKE_SOURCE_DIR}/Shared/Sources" "${PROJECT_SHARED_SOURCES}")
# Организуем файлы по группам
organize_source_files(${CMAKE_CURRENT_LIST_DIR} "${PROJECT_SOURCES}")

target_sources(OksEngine
    PUBLIC
    ${PROJECT_SOURCES}
    ${PROJECT_SHARED_SOURCES}
)

find_package(Boost REQUIRED COMPONENTS asio)

find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

find_package(Stb REQUIRED)
target_include_directories(OksEngine PRIVATE ${Stb_INCLUDE_DIR})

find_package(Freetype REQUIRED)
target_include_directories(OksEngine PRIVATE ${FREETYPE_INCLUDE_DIRS})

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(RapidJSON REQUIRED)

target_include_directories(OksEngine PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(OksEngine PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(OksEngine PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(OksEngine PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto
)

target_link_libraries(OksEngine
    PUBLIC
    Boost::asio
    SFML::Audio
    SFML::System
    yaml-cpp::yaml-cpp
    ${FREETYPE_LIBRARIES}
    ImGui
    Geometry
    ECS2
    LuaLayer
    Config
    Render
    Physics
    OS
    UI
    Resources
)


target_include_directories(OksEngine PRIVATE "${VCPKG_INSTALLED_DIR}/include")
target_link_libraries(OksEngine PRIVATE "${VCPKG_INSTALLED_DIR}/x64-windows/lib/TgBot.lib")

target_include_directories(OksEngine
    PUBLIC
    "${CMAKE_SOURCE_DIR}/Shared/Sources"
    ${CMAKE_CURRENT_LIST_DIR})
