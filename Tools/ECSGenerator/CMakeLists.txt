cmake_minimum_required(VERSION  3.26.4)

# Добавляем исполнительный файл и связываем с библиотеками
add_executable(ECSGenerator 
    Sources/GenerateCXX/main.cpp)

set_target_properties(ECSGenerator
    PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS
        "-workDir C:/Users/User/Desktop/OksEngine/Sources/")

#nd_package(libcgraph CONFIG REQUIRED)
# target_link_libraries(ECS
#     PUBLIC
#     graphviz::libcgraph
#     )
 # find_package(libcgraph CONFIG REQUIRED)
 # include_directories(${CMAKE_CURRENT_LIST_DIR}/Graphviz-12.1.0-win64/include/)
 # link_directories(
  #    ${CMAKE_CURRENT_LIST_DIR}/lib/)
 # target_link_libraries(ECS
    # ${CMAKE_CURRENT_LIST_DIR}/Graphviz-12.2.1-win64/lib/cdt.lib
    # ${CMAKE_CURRENT_LIST_DIR}/Graphviz-12.2.1-win64/lib/cgraph.lib
    # ${CMAKE_CURRENT_LIST_DIR}/Graphviz-12.2.1-win64/lib/gvc.lib
    #  ${CMAKE_CURRENT_LIST_DIR}/cdt.lib
    #  ${CMAKE_CURRENT_LIST_DIR}/cgraph.lib
    #  ${CMAKE_CURRENT_LIST_DIR}/gvc.lib
 # )

# set(GRAPHVIZ_DIR "C:/Users/User/Desktop/OksEngine/Tools/ECSGenerator/Graphviz-12.0.0-win64/")
# file(GLOB GVC_LIBS "${GRAPHVIZ_DIR}/lib/*.lib")

# include_directories(ECSGenerator "${GRAPHVIZ_DIR}/include/")
# target_link_libraries(ECSGenerator ${GVC_LIBS})

# Укажите статическую библиотеку
# add_library(graphviz STATIC IMPORTED)
# set_target_properties(graphviz PROPERTIES IMPORTED ${GVC_LIBS})

# the `pkg_check_modules` function is created with this call
find_package(PkgConfig REQUIRED) 

# these calls create special `PkgConfig::<MODULE>` variables
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

