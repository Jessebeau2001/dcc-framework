#include <Arduino.h>
#include "HAL/bitbang.h"

#include "protocol/dcc_writer.h"

inline void write_byte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
		Serial.print((byte >> i) & 1);
	}
}

inline void write_bit(bool bit) {
    Serial.print(bit ? "1" : "0");
}

inline void print_timing(uint8_t t) {
	Serial.print(t);
	Serial.print(" ");
}

using proto::dcc::TimingAdapter;
using proto::dcc::DccController;

void setup() {
    // bitbang::init();

    Serial.begin(9600);
    while (!Serial.available()) { }

	auto controller = DccController<TimingAdapter<58, 100, print_timing>>();

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

	write_byte(cmd1.instruction.value);
	Serial.println();
	Serial.println();
	
	controller.send_command(cmd1);
	// out:
	// 11111111 11111111 0 11001100 0 01110101 0 10111001 1
}

// constexpr uint8_t FRAME[8] = {50U, 50U, 50U, 50U, 50U, 50U, 50U, 50U }; 

void loop() {
    // if (bitbang::available_for_write() > 8) {
    //     bitbang::write_frame(FRAME, 2);
    // }
}

// #include "measure.hpp"