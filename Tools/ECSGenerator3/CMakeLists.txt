cmake_minimum_required(VERSION  3.26.4)

function(get_source_files FILE_LIST)
    set(TEMP_FILES "")
    foreach(DIR ${ARGN})
        file(GLOB_RECURSE DIR_FILES "${DIR}/*.cpp" "${DIR}/*.hpp")
        list(APPEND TEMP_FILES ${DIR_FILES})
    endforeach()
    #message(STATUS "Total files: ${TEMP_FILES}")
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
get_source_files(
    PROJECT_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Sources"
        "${CMAKE_CURRENT_LIST_DIR}/Packages" 
    )

# Организуем файлы по группам
organize_source_files(${CMAKE_CURRENT_LIST_DIR} "${PROJECT_SOURCES}")

add_executable(ECSGenerator3)

#Generate cpp includes with cmake files

function(GenerateTemplates TEMPLATE_PATH DEST_PATH)
    # Пропускаем первые два аргумента (TEMPLATE_PATH и DEST_PATH)
    set(INDEX 2)
    while(INDEX LESS ${ARGC})
        math(EXPR NAME_INDEX "${INDEX}")
        math(EXPR PATH_INDEX "${INDEX} + 1")
        
        list(GET ARGV ${NAME_INDEX} VAR_NAME)
        list(GET ARGV ${PATH_INDEX} FILE_PATH)
        
        message(STATUS "Processing: ${VAR_NAME} = ${FILE_PATH}")
        
        if(EXISTS "${FILE_PATH}")
            file(READ "${FILE_PATH}" CONTENT)
            #string(REPLACE "\\" "\\\\" CONTENT "${CONTENT}")
            #string(REPLACE "\"" "\\\"" CONTENT "${CONTENT}")
            set(${VAR_NAME} "${CONTENT}")
        else()
            message(WARNING "File not found: ${FILE_PATH} — using empty content")
            set(${VAR_NAME} "")
        endif()
        
        math(EXPR INDEX "${INDEX} + 2")
    endwhile()
    
    # Создаём выходную директорию, если её нет
    get_filename_component(DEST_DIR "${DEST_PATH}" DIRECTORY)
    file(MAKE_DIRECTORY "${DEST_DIR}")
    
    configure_file(
        "${TEMPLATE_PATH}"
        "${DEST_PATH}"
        @ONLY
    )
endfunction()


GenerateTemplates(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeTemplates/Help.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/CMakeTemplates_generated/Help.h"
    "CONTENT" "${CMAKE_CURRENT_SOURCE_DIR}/CMakeTemplates/Help.cmake" 
)

GenerateTemplates(
    "${CMAKE_CURRENT_SOURCE_DIR}/CPPTemplates/Project.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/auto_Project.hpp"
    "PROJECT_HPP_CONTENT" "${CMAKE_CURRENT_SOURCE_DIR}/CPPTemplates/Project.hpp"
    "PROJECT_CPP_CONTENT" "${CMAKE_CURRENT_SOURCE_DIR}/CPPTemplates/Project.cpp"
)


find_package(PkgConfig REQUIRED) 
target_compile_options(ECSGenerator3 PUBLIC "/MP" "/bigobj")
pkg_check_modules(GRAPHVIZ REQUIRED libgvc)
message(STATUS "${GRAPHVIZ_INCLUDE_DIRS}")



target_include_directories(
    ECSGenerator3  
    PUBLIC
        ${GRAPHVIZ_INCLUDE_DIRS}
        "${CMAKE_BINARY_DIR}/CMakeTemplates_generated")
    
message(STATUS "${GRAPHVIZ_LIBRARY_DIRS}")
target_link_directories(ECSGenerator3 PUBLIC ${GRAPHVIZ_LIBRARY_DIRS})

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

llvm_map_components_to_libnames(LLVM_LIBS core irreader support)

set(CLANG_LIBS
    clangFormat
    clangToolingCore
    clangBasic
    clangLex
    clangSerialization
    clangRewrite
    clangAST
    clangSupport
    clangFrontend
    clangDriver
    LLVMSupport
)

set_target_properties(ECSGenerator3
        PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS
        "$<$<CONFIG:Debug>: -ecs_project_file D:/OksEngine/Tools/TgBot/ECSGenerator3.ecs_project>$<$<CONFIG:Release>: -ecs_project_file D:/OksEngine/Tools/TgBot/ECSGenerator3.ecs_project>$<$<CONFIG:RelWithDebInfo>: -ecs_project_file D:/OksEngine/Tools/TgBot/ECSGenerator3.ecs_project>"
    )

target_include_directories(ECSGenerator3
    PUBLIC
        ${LLVM_INCLUDE_DIRS} 
        ${CLANG_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}/Sources/)

target_sources(ECSGenerator3
    PUBLIC
        ${PROJECT_SOURCES}

    PRIVATE
    )

target_include_directories(ECSGenerator3
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})


target_link_libraries(ECSGenerator3
    PUBLIC
    ${GRAPHVIZ_LIBRARIES}
    ${CLANG_LIBS}
    ${LLVM_LIBS}
    LuaLayer
    Resources
    Config
    ECS2
    ImGui
    )

message("ZLIB_FOUND: ${ZLIB_FOUND}")
message("ZLIB_INCLUDE_DIRS: ${ZLIB_INCLUDE_DIRS}")
message("ZLIB_LIBRARIES: ${ZLIB_LIBRARIES}")
message("ZLIB_LIBRARY: ${ZLIB_LIBRARY}")
message("ZLIB_LIBRARY: ${ZLIB_LIBRARY_RELEASE}")
message("ZLIB_LIBRARY_DEBUG: ${ZLIB_LIBRARY_DEBUG}")

add_custom_command(TARGET ECSGenerator3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ZLIB_LIBRARY_RELEASE}"
    "${ZLIB_LIBRARY_DEBUG}"
    "${GRAPHVIZ_PREFIX}/bin/cdt.dll"
    "${GRAPHVIZ_PREFIX}/bin/cgraph.dll"
    "${GRAPHVIZ_PREFIX}/bin/config6"
    "${GRAPHVIZ_PREFIX}/bin/gvc.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_core.dll" 
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_dot_layout.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_gd.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_gdiplus.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_kitty.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_neato_layout.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_pango.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_rsvg.dll "
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_vt.dll"
    "${GRAPHVIZ_PREFIX}/bin/gvplugin_webp.dll"
    "${GRAPHVIZ_PREFIX}/bin/pathplan.dll"
    "${GRAPHVIZ_PREFIX}/bin/xdot.dll"
     $<TARGET_FILE_DIR:ECSGenerator3>)

