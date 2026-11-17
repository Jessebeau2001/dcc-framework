#pragma once

#include <types.h>
#include <HAL/register.h>
#include <concepts.h>

namespace HAL {
using namespace types;
using namespace traits;

template <typename T>
struct DynamicPin {
    volatile T* addr;
    T mask;
};

template <typename T>
struct DataView {
    using ValueType = T;

    T const* addr;
    T mask;
};

template <typename View>
inline bool read_bit(const View& view) {
    return (*view.addr & view.mask) != 0;
}

// enable_if_t<is_same_v<typename Pin::PORT::BaseType, u8>, int> = 0
template <typename Pin>
constexpr DynamicPin<typename Pin::PORT::BaseType> make_dynamic() {
    using T = typename Pin::PORT::BaseType;
    return DynamicPin<T>{
        .addr = reinterpret_cast<volatile T*>(Pin::PortReg::ADDRESS),
        .mask = Pin::PORT::MASK
    };
}

} // namespace HAL