cmake_minimum_required(VERSION  3.26.4)
include(auto_Packages.cmake)
get_source_files("${CMAKE_CURRENT_LIST_DIR}/Packages" SOURCES)
organize_source_files(${CMAKE_CURRENT_LIST_DIR} "${SOURCES}")
add_executable(PMTest)
target_compile_options(PMTest PUBLIC "/MP" "/bigobj")
target_include_directories(PMTest PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/Packages/${CMAKE_CURRENT_LIST_DIR}/Sources/)
target_sources(PMTest PUBLIC ${SOURCES}${PACKAGES_SOURCES})

