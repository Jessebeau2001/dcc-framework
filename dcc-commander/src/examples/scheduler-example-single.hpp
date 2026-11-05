#pragma once

#include <Arduino.h>
#include <types.h>
#include <HAL/bitbang.hpp>
#include <protocol/dcc.hpp>
#include <Arduino.h>

using namespace types;

using dcc::DccSerializer;
using dcc::Command;
using dcc::Instruction;
using dcc::SpeedStep;
using dcc::Direction;
using dcc::DccScheduler;

auto scheduler = DccScheduler<bitbang::Writer>();

Command cmd = {
	.address = 0,
	.instruction = Instruction{
		SpeedStep::Step28,
		Direction::FORWARD,
		0b01
	}
};

void update_addr() {
	scheduler.remove(cmd.address);

	cmd.address += 1;
	// if (cmd.address >= 128) {
	// 	cmd.address = 0;
	// }

	scheduler.schedule(cmd);

	Serial.print("Updated command address to ");
	Serial.println(cmd.address);
}

void setup() {
	digitalWrite(7, HIGH); // Set hat enable pin
	pinMode(7, OUTPUT);
	
	pinMode(9, OUTPUT);		// Set disable sleep pin
	digitalWrite(9, HIGH);
	
	Serial.begin(9600);
    bitbang::begin();
	scheduler.schedule(cmd);
}

void loop() {
	scheduler.update();

	if (Serial.available() > 0) {
    	char incomingByte = Serial.read();
		if (incomingByte == '\n') {
			update_addr();
		}
	}
}