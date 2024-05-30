cmake_minimum_required(VERSION  3.26.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(Datastructures STATIC)
set(SourcesDir "Sources/")

file(GLOB_RECURSE PUBLIC_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/*.hpp
    ${SourcesDir}/*.h
)

file(GLOB_RECURSE
 PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    ${SourcesDir}/*.cpp
    ${SourcesDir}/*.c
)

target_sources(Datastructures
    PUBLIC
        ${PUBLIC_SOURCES}
        # OS.Assert.hpp
        # OS.Clock.hpp
        # OS.Exception.hpp
        # OS.FileSystem.BinaryFile.hpp
        # OS.FileSystem.File.hpp
        # OS.FileSystem.hpp
        # OS.FileSystem.TextFile.hpp
        # OS.hpp
        # OS.Logger.hpp
        # OS.Profiler.hpp
    PRIVATE
        ${PRIVATE_SOURCES}
        # OS.Exception.cpp
    )

        
source_group("Sources"
FILES
${PUBLIC_SOURCES}
${PRIVATE_SOURCES})


target_include_directories(Datastructures
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/${SourcesDir})
    
        
find_package(Boost CONFIG REQUIRED)
target_link_libraries(Datastructures
    PUBLIC
        OS
        #Boost::Log
        ${Boost_LIBRARIES}
        )

# Boost::headers
# Boost::atomic
# Boost::chrono
# Boost::container
# Boost::context
# Boost::coroutine
# Boost::date_time
# Boost::exception
# Boost::fiber
# Boost::filesystem
# Boost::graph
# Boost::graph_parallel
# Boost::iostreams
# Boost::locale
# Boost::log
# Boost::log_setup
# Boost::math_c99
# Boost::math_c99f
# Boost::math_c99l
# Boost::math_tr1
# Boost::math_tr1f
# Boost::math_tr1l
# Boost::mpi
# Boost::mpi_python
# Boost::nowide
# Boost::numpy
# Boost::prg_exec_monitor
# Boost::program_options
# Boost::python
# Boost::random
# Boost::regex
# Boost::serialization
# Boost::stacktrace_addr2line
# Boost::stacktrace_backtrace
# Boost::stacktrace_basic
# Boost::stacktrace_noop
# Boost::system
# Boost::test_exec_monitor
# Boost::thread
# Boost::timer
# Boost::type_erasure
# Boost::unit_test_framework
# Boost::wave
# Boost::wserialization

add_subdirectory(Tests)