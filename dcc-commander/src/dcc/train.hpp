#pragma once

#include <dcc/types.h>

namespace dcc {
    enum class DriveState : u8 {
        Docked,
        Driving,
        Lost
    };

    struct TrainState {
        DriveState state;
        Direction direction;
        u8 current_station;
        u8 target_station;
    };

    class TrackScanner {
    public:
        void update() {

        }
        
        void derive_state(TrainState& train) {

        }
    };
}
