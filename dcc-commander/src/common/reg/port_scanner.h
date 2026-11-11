#pragma once

#include <types.h>
#include <stdint.h>

using namespace types;

template <typename Reg>
struct PortScanner {
    using ValueType = typename Reg::ValueType;
    
    // State values are dependant on register width
    struct State {
        ValueType value;
        ValueType cnt0;
        ValueType cnt1;
    };
    
    /* This uses a parallel vertical counter algorithm to debounce - Example:
        delta = reading ^ debounced             | 1 ^ 0 = 1         (disagreement detected)
        cnt0 = ~cnt0 & delta                    |  ~0 & 1 = 1       (cnt0 updates to 1)
        cnt1 = (cnt0 ^ cnt1) & delta            | (1 ^ 0) & 1 = 1   (cnt1 updates to 1)
        stable_mask = ~(cnt0 | cnt1) & delta    | ~(1 | 1) & 1 = 0  (persistence threshold not yet met)
        debounced ^= stable_mask;               | 0 ^= 0 = 0        (stable state remains unchanged on tick 1
    */

    static inline void update(State& s) {
        u8 delta = Reg::read() ^ s.value;               // XOR delta of known good and new data

        s.cnt0 = ~s.cnt0 & delta;                       // Count up 1 in counters for every delta bit
        s.cnt1 = (s.cnt0 ^ s.cnt1) & delta;             // Same for counter 1

        u8 stable_mask = ~(s.cnt0 | s.cnt1) & delta;    // Isolate bits that counted to 2 succesfully
        s.value ^= stable_mask;                         // Merge the stable bits onto the known good value
    }
};
