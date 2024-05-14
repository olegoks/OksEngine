cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(Common STATIC)

file(GLOB PUBLIC_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.hpp
    *.h
)

file(GLOB PRIVATE_SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    LIST_DIRECTORIES false
    *.cpp
    *.c
)

target_sources(Common
PUBLIC
    # Common.DataStructures.Graph.hpp
    # Common.DataStructures.hpp
    # Common.DataStructures.Vector.hpp
    # Common.Exception.hpp

    # Common.Format.hpp
    # Common.hpp
    # Common.Identifier.hpp
    # # Common.Memory.Allocator.hpp
    # # Common.Memory.BlockAllocator.hpp
    # # Common.Memory.Concepts.hpp
    # # Common.Memory.Exception.hpp
    # # Common.Memory.hpp
    # # Common.Memory.MemoryControlBlock.hpp
    # # Common.Memory.PoolAllocator.hpp
    # # Common.Memory.RefsCounter.hpp
    # # Common.Memory.SharedPointer.hpp
    # # Common.MemoryManager.hpp

    # Common.StringView.hpp
    # Common.Types.hpp
    # Common.VersionedMap.hpp
    ${PUBLIC_SOURCES}
PRIVATE
    ${PRIVATE_SOURCES}
    # # Common.StringView.cpp
    # # Common.Format.cpp
    )

        
    source_group("Sources"
    FILES
    ${PUBLIC_SOURCES}
    ${PRIVATE_SOURCES})



target_include_directories(Common
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
