#pragma once 

#include <Common.Types.hpp>

namespace Common {

    constexpr Common::UInt64 operator "" _KB(Common::UInt64 kbNumber) noexcept {
        return kbNumber * 1'024;
    };
    
    constexpr Common::UInt64 operator "" _MB(Common::UInt64 mbNumber) noexcept {
        return mbNumber * 1_KB * 1_KB;
    };

    constexpr Common::UInt64 operator "" _GB(Common::UInt64 gbNumber) noexcept {
        return gbNumber * 1_MB * 1_KB;
    };
}