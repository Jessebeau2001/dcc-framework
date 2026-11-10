#pragma once

#include <types.h>
#include <stdint.h>

using namespace types;


template <typename Reg>
class PortScanner {
private:
    u8 debounced = 0;
    u8 cnt0 = 0;
    u8 cnt1 = 0;

    /* This uses a parallel vertical counter algorithm to debounce - Example:
        delta = reading ^ debounced             | 1 ^ 0 = 1         (disagreement detected)
        cnt0 = ~cnt0 & delta                    |  ~0 & 1 = 1       (cnt0 updates to 1)
        cnt1 = (cnt0 ^ cnt1) & delta            | (1 ^ 0) & 1 = 1   (cnt1 updates to 1)
        stable_mask = ~(cnt0 | cnt1) & delta    | ~(1 | 1) & 1 = 0  (persistence threshold not yet met)
        debounced ^= stable_mask;               | 0 ^= 0 = 0        (stable state remains unchanged on tick 1
    */

public:
    inline void update() {
        u8 delta = Reg::read() ^ debounced;         // XOR delta of known good and new data

        cnt0 = ~cnt0 & delta;                       // Count up 1 in counters for every delta bit
        cnt1 = (cnt0 ^ cnt1) & delta;               // Same for counter 1

        u8 stable_mask = ~(cnt0 | cnt1) & delta;    // Isolate bits that counted to 2 succesfully
        debounced ^= stable_mask;                   // Merge the stable bits onto the known good value
    }

    inline u8 read() const {
        return debounced;
    }
};

// Example
// #define ADDR_GPIO_B 0x23
// auto scanner = PortScanner<ReadonlyRegister<ADDR_GPIO_B>>