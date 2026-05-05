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

add_executable(TgBot)

find_package(PkgConfig REQUIRED) 
target_compile_options(TgBot PUBLIC "/MP" "/bigobj")
pkg_check_modules(GRAPHVIZ REQUIRED libgvc)
message(STATUS "${GRAPHVIZ_INCLUDE_DIRS}")

target_include_directories(TgBot  PUBLIC ${GRAPHVIZ_INCLUDE_DIRS})
    
message(STATUS "${GRAPHVIZ_LIBRARY_DIRS}")
target_link_directories(TgBot PUBLIC ${GRAPHVIZ_LIBRARY_DIRS})

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

set_target_properties(TgBot
        PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS
        "$<$<CONFIG:Debug>: -ecs_project_file D:/OksEngine/Tools/TgBot/ecs_generator3.ecs_project>$<$<CONFIG:Release>: -cfg ${CMAKE_CURRENT_LIST_DIR}/../config.lua -scn ${CMAKE_CURRENT_LIST_DIR}/../Resources/Scenes/OksEngine.EmptyScene.scn>$<$<CONFIG:RelWithDebInfo>: -cfg ${CMAKE_CURRENT_LIST_DIR}/../config.lua -scn resource:/Scenes/OksEngine.EmptyScene.scn>"
    )

target_include_directories(TgBot
    PUBLIC
        ${LLVM_INCLUDE_DIRS} 
        ${CLANG_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}/Sources/)

target_sources(TgBot
    PUBLIC
        ${PROJECT_SOURCES}

    PRIVATE
    )

target_include_directories(TgBot
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})


target_link_libraries(TgBot
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

add_custom_command(TARGET TgBot POST_BUILD
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
     $<TARGET_FILE_DIR:TgBot>)

