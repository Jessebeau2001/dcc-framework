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
using proto::dcc::Command;
using proto::dcc::Instruction;
using proto::dcc::Speed;
using proto::dcc::Light;
using proto::dcc::Direction;

void setup() {
    // bitbang::init();

    // Serial.begin(9600);
    // while (!Serial.available()) { }

	auto controller = DccController<DccWriter<TimingAdapter<58, 100, print_timing>>>();

	Command cmd1 = {
		.address = 0b11001100,
		.instruction = Instruction{
			Speed::Step5,
			Light::ON,
			Direction::FORWARD,
			0b01
		}
	};
	
	controller.send_command(cmd1);
}

void loop() {
    // if (bitbang::available_for_write() > 8) {
    //     bitbang::write_frame(FRAME, 2);
    // }
}
