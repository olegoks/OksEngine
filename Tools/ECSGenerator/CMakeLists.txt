cmake_minimum_required(VERSION  3.26.4)

# Добавляем исполнительный файл и связываем с библиотеками
add_executable(ECSGenerator 
    Sources/GenerateCXX/main.cpp)

set_target_properties(ECSGenerator
    PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS
        "-workDir C:/Users/User/Desktop/OksEngine/Sources/")

find_package(PkgConfig REQUIRED) 

pkg_check_modules(GRAPHVIZ REQUIRED libgvc)
message(STATUS "${GRAPHVIZ_INCLUDE_DIRS}")
target_include_directories(ECSGenerator  PUBLIC ${GRAPHVIZ_INCLUDE_DIRS})

message(STATUS "${GRAPHVIZ_LIBRARY_DIRS}")
target_link_directories(ECSGenerator PUBLIC ${GRAPHVIZ_LIBRARY_DIRS})

get_cmake_property(_variableNames VARIABLES)

foreach (_variableName ${_variableNames})
message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

target_link_libraries(ECSGenerator
    PUBLIC
    ${GRAPHVIZ_LIBRARIES}
    LuaLayer
    Resources
    ECS
    )

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
"${GRAPHVIZ_PREFIX}/bin/xdot.dll" $<TARGET_FILE_DIR:ECSGenerator>)

