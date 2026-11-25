#pragma once

#include <dcc/types.h>
#include <dcc/station.hpp>

namespace train {
    enum class DriveState : u8 {
        Docked,
        DrivingForward,
        DrivingBackward
    };

    enum class RelativeDirection {
        Left,
        Center,
        Right
    };

    struct TrainState {
        // Extrapolated from the sensors
        u8                current_station_id;
        RelativeDirection current_station_rel;

        // Set as detirmened by controller
        DriveState drive_state;
        u8         target_station;
    };


    template <u8 Size>
    class TrackScanner {
        const Station* stations[Size];

    public:
        constexpr TrackScanner(const Station *const (&init)[Size]) : stations{} {
            for (u8 i = 0; i < Size; ++i) {
                stations[i] = init[i];
            }
        }

        void write_train(TrainState& train) const {
            for (u8 i = 0; i < Size; i++) {
                const Station& current_station = *stations[i];
                auto occupancy = station_check_occupancy(current_station);

                switch (occupancy) {
                case StationOcupancy::EMPTY:
                    continue;
                case StationOcupancy::LEFT:
                    train.current_station_rel = RelativeDirection::Left;
                    break;
                case StationOcupancy::OCCUPIED:
                    train.current_station_rel = RelativeDirection::Center;
                    break;
                case StationOcupancy::RIGHT:
                    train.current_station_rel = RelativeDirection::Right;
                    break;
                }

                train.current_station_id = current_station.id;
            }
        }
    };

    template<typename StationType>
    static constexpr void resolve_position(TrainState& train) {
        auto occupancy = station_static_scan<typename StationType::Sensors>();

        switch (occupancy) {
        case StationOcupancy::EMPTY:
            return;
        case StationOcupancy::LEFT:
            train.current_station_rel = RelativeDirection::Left;
            break;
        case StationOcupancy::OCCUPIED:
            train.current_station_rel = RelativeDirection::Center;
            break;
        case StationOcupancy::RIGHT:
            train.current_station_rel = RelativeDirection::Right;
            break;
        }

        train.current_station_id = StationType::ID;
    }

    template<typename... Stations>
    struct Track {
        static constexpr void resolve_train_position(TrainState& out) {
            (train::resolve_position<Stations>(out), ...);
        }
    };
}
