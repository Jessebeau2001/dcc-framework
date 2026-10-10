#include <Arduino.h>
#include "HAL/bitbang.hpp"

#include "protocol/dcc.hpp"

using proto::dcc::TimingAdapter;
using proto::dcc::DccSerializer;
using proto::dcc::DccController;
using proto::dcc::Command;
using proto::dcc::Instruction;
using proto::dcc::Speed;
using proto::dcc::Light;
using proto::dcc::Direction;
using proto::dcc::SerialWriter;


auto controller = DccController<DccSerializer<bitbang::Writer>>();

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
	// Serial.begin(9600);
	// while (!Serial.available()) { }

	controller.send_command(cmd1);
}

void loop() {
	// if (bitbang::available_for_write() > 40) {
	// 	controller.send_command(cmd1);
    // }
}
