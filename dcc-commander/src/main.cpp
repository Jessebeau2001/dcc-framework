#include <Arduino.h>
#include "HAL/bitbang.hpp"

#include "protocol/dcc.hpp"

using proto::dcc::TimingAdapter;
using proto::dcc::DccSerializer;
using proto::dcc::DccController;
using proto::dcc::Command;
using proto::dcc::Instruction;
using proto::dcc::Speed;
using proto::dcc::Direction;
using proto::dcc::SerialWriter;


auto controller = DccController<DccSerializer<bitbang::Writer>>();

Command cmd1 = {
	.address = 55,
	.instruction = Instruction{
		Speed::Step6,
		Direction::FORWARD,
		0b01
	}
};

void setup() {
    bitbang::begin();
}

void loop() {
	if (bitbang::bytes_available_for_write() > 40) {
		cmd1.address++;
		controller.send_command(cmd1);
	}
	delay(500);
}

/*
a packet is 3 bytes + 2 preamble bytes + 4 seperators = 44 bits
worst case bit duration is 100us for a zero:

packet transmission time
	= 44 bits * 2 pulses * 100 µs
	= 8800 µs

packets per second
	= 1 second     / transmission time in seconds
	= 1'000'000 µs / transmission time in µs
	= 1'000'000 µs / 8800 µs
	= ~113 packets / second
*/