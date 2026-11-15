#pragma once

#include <types.h>
#include <HAL/register.h>
#include <concepts.h>

namespace HAL {
using namespace types;
using namespace traits;

template <typename T>
struct DynamicPin
{
    volatile T* port;
    T mask;
};

// enable_if_t<is_same_v<typename Pin::PORT::BaseType, u8>, int> = 0
template <typename Pin>
constexpr DynamicPin<typename Pin::PORT::BaseType> make_dynamic() {
    using T = typename Pin::PORT::BaseType;
    return DynamicPin<T>{
        .port = reinterpret_cast<volatile T*>(Pin::PortReg::ADDRESS),
        .mask = Pin::PORT::MASK
    };
}


template <typename T>
void io_write(DynamicPin<T> pin, u8 value) {
    // something
}

} // namespace HAL