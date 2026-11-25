#pragma once

#include <types.h>
#include <stddef.h>

using namespace types;

template<typename Reg, size_t Index>
struct View {
    using BaseType = typename Reg::ValueType;

    static_assert(Index >= 0, "Bit index cannot be negative");
    static_assert(Index < (sizeof(BaseType) * 8), "Bit Index is out of range for type");
    
    static constexpr BaseType MASK = static_cast<BaseType>(1U) << Index;

    static constexpr bool is_set() {
        return (Reg::read() & MASK) != 0;
    }
};

// TODO: Rename to RWHandle?
template<typename Reg, size_t Index>
struct BitHandle : View<Reg, Index> {
    static constexpr auto MASK = View<Reg, Index>::MASK;

    static constexpr void set() {
        Reg::write(Reg::read() | MASK);
    }

    static constexpr void clear() {
        Reg::write(Reg::read() & ~MASK);
    }

    static constexpr void write(bool value) {
        if (value) set();
        else clear();
    }
};

template<typename Reg, size_t Index>
struct W1CHandle : View<Reg, Index> {
    static constexpr auto MASK = View<Reg, Index>::MASK;

    static constexpr void clear() {
        Reg::write(MASK);
    }
};

