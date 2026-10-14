#pragma once

#include <Arduino.h>
#include <types.h>
#include <HAL/bitbang.hpp>
#include <protocol/dcc.hpp>

using namespace types;

using dcc::DccSerializer;
using dcc::Command;
using dcc::Instruction;
using dcc::SpeedStep;
using dcc::Direction;
using dcc::DccScheduler;

auto scheduler = DccScheduler<bitbang::Writer>();

uint32_t previous_time = 0; 
uint16_t delay_ms = 1000;

u8 num_actions = 0;
u8 rest = 0;
bool reverse = 0;

Command cmd = {
	.address = 0,
	.instruction = Instruction{
		SpeedStep::Step6,
		Direction::FORWARD,
		0b01
	}
};

void update_scheduling() {
	if (rest > 0) {
		Serial.print("Resting for ");
		Serial.println(rest);

		rest--;
		return;
	}

	u8 addr = 55 + (10 * num_actions);

	if (reverse == 0) {
		cmd.address = addr;
		scheduler.schedule(cmd);
	} else {
		scheduler.remove(addr);
	}
	
	num_actions += 1;
	if (num_actions > 4) {
		num_actions = 0;
		reverse = !reverse;
		rest = 8;
	}
}

void setup() {
    bitbang::begin();
}

void loop() {
	uint32_t current_time = millis();

	if (current_time - previous_time >= delay_ms) {
		previous_time = current_time;
		update_scheduling();
	}

	scheduler.update();
}