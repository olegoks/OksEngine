#pragma once 

#include <any>

#include <Common.hpp>

namespace Memory {

    using AllocationFunction = std::function<void*(
        std::any        userData, 
        Common::Size    size,
        Common::Size    alignment)>;

    using ReallocationFunction = std::function<void*(
        std::any        userData, 
        void*           original,
        Common::Size    size,
        Common::Size    alignment)>;

    using FreeFunction = std::function<void(
        std::any        userData,
        void*           memory)>;

    struct AllocationCallbacks {
        std::any                userData;

        AllocationFunction      allocationCallback = [](
            std::any        userData,
            Common::Size    size,
            Common::Size    alignment) {
                return std::malloc(size);
            };

        ReallocationFunction    reallocationCallback = [](
            std::any        userData,
            void*           original,
            Common::Size    size,
            Common::Size    alignment) { OS::NotImplemented(); return nullptr; };

        FreeFunction            freeCallback = [](
            std::any        userData,
            void* memory) { std::free(memory); };
    };

}