cmake_minimum_required(VERSION  3.26.4)

# Добавляем исполнительный файл и связываем с библиотеками
add_executable(ECSGenerator Sources/GenerateCXX/main.cpp)

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

#set(GRAPHVIZ_DIR "C:/Users/User/Desktop/OksEngine/Tools/ECSGenerator/Graphviz-12.0.0-win64/")
#file(GLOB GVC_LIBS "${GRAPHVIZ_DIR}/lib/*.lib")
#target_link_libraries(ECS ${GVC_LIBS})
#include_directories("${GRAPHVIZ_DIR}/include/")

target_link_libraries(ECSGenerator
    PUBLIC
    LuaLayer
    Resources
    ECS
    )
