cmake_minimum_required(VERSION  3.26.4)

function(get_source_files DIRECTORY FILE_LIST)
    file(GLOB_RECURSE TEMP_FILES "${DIRECTORY}/*.cpp" "${DIRECTORY}/*.hpp")
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

add_executable(ECSGenerator)

find_package(PkgConfig REQUIRED) 

pkg_check_modules(GRAPHVIZ REQUIRED libgvc)
message(STATUS "${GRAPHVIZ_INCLUDE_DIRS}")
target_include_directories(ECSGenerator  PUBLIC ${GRAPHVIZ_INCLUDE_DIRS})
    
message(STATUS "${GRAPHVIZ_LIBRARY_DIRS}")
target_link_directories(ECSGenerator PUBLIC ${GRAPHVIZ_LIBRARY_DIRS})
    

# Поиск LLVM и Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

# Добавляем необходимые библиотеки
llvm_map_components_to_libnames(LLVM_LIBS core irreader support)


# Добавляем необходимые библиотеки
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

set_target_properties(ECSGenerator
    PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS
        "-workDir ${CMAKE_CURRENT_LIST_DIR}/../../Sources/Engine/Sources -cfg ${CMAKE_CURRENT_LIST_DIR}/../../Sources/Engine/Sources/ecs_gen_config.lua")
target_include_directories(ECSGenerator
    PUBLIC
        ${LLVM_INCLUDE_DIRS} 
        ${CLANG_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}/Sources/)

target_sources(ECSGenerator
    PUBLIC
        ${PROJECT_SOURCES}

    PRIVATE
    )


target_link_libraries(ECSGenerator
    PUBLIC
    ${GRAPHVIZ_LIBRARIES}
    ${CLANG_LIBS}
    ${LLVM_LIBS}
    LuaLayer
    Resources
    Config
    )
    

# Log all cmake variables.

get_cmake_property(_variableNames VARIABLES)

foreach (_variableName ${_variableNames})
message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

add_custom_command(TARGET ECSGenerator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
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
     $<TARGET_FILE_DIR:ECSGenerator>)

