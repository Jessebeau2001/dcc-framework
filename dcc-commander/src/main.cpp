#include <Arduino.h>
#include "HAL/bitbang.h"

#include "protocol/dcc_writer.h"
#include "common/bit_ring_buffer.hpp"

inline void print_timing(uint8_t t) {
	Serial.print(t);
	Serial.print(" ");
}

using proto::dcc::TimingAdapter;
using proto::dcc::DccWriter;
using proto::dcc::DccController;
using proto::dcc::Command;
using proto::dcc::Instruction;
using proto::dcc::Speed;
using proto::dcc::Light;
using proto::dcc::Direction;

auto controller = DccController<DccWriter<TimingAdapter<58, 100, bitbang::write_byte>>>();

Command cmd1 = {
	.address = 0b11001100,
	.instruction = Instruction{
		Speed::Step5,
		Light::ON,
		Direction::FORWARD,
		0b01
	}
};

void setup() {
    // bitbang::init();

	pinMode(PIN6, OUTPUT);
	analogWrite(PIN6, 127);
}

void loop() {
	// if (bitbang::available_for_write() > 40) {
	// 	controller.send_command(cmd1);
    // }
}
