#include <Arduino.h>
#include "HAL/bitbang.h"

#include "protocol/dcc_writer.h"

inline void print_timing(uint8_t t) {
	// Serial.print(t);
	// Serial.print(" ");
}

using proto::dcc::TimingAdapter;
using proto::dcc::DccWriter;
using proto::dcc::DccController;

void setup() {
    // bitbang::init();

    // Serial.begin(9600);
    // while (!Serial.available()) { }

	auto controller = DccController<DccWriter<TimingAdapter<58, 100, print_timing>>>();

	proto::dcc::Command cmd1 = {
		.address = 0b11001100,
		.instruction = {
			.fields = {
				.speed = 0b0101,     // Step 5
				.light = 1,          // Light On
				.direction = 1,      // Forward
				.idata = 0b01        // Instruction data
			} // 01110101
		}
	};
	
	controller.send_command(cmd1);
}

// constexpr uint8_t FRAME[8] = {50U, 50U, 50U, 50U, 50U, 50U, 50U, 50U }; 

void loop() {
    // if (bitbang::available_for_write() > 8) {
    //     bitbang::write_frame(FRAME, 2);
    // }
}

// #include "measure.hpp"