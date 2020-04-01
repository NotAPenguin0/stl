#include <stl/types.hpp>

namespace stl {

namespace literals {

constexpr uint32_t operator""_u32(unsigned long long val) {
    return static_cast<uint32_t>(val);
}

}

}