#pragma once

#include <platform/chip.h>
#include <common/handle.h>
#include <dcc/dcc-banger.hpp>
#include <dcc/dcc.hpp>
#include <Arduino.h>

using platform::avr::Timer1;

template <typename Pin>
struct Timer1Handle {
    static inline void configure() {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        HAL::io_configure<Pin>(HAL::IOMode::Output);
        
        // Reset Timer 1 Registers
        Timer1::TCCRA::clear();
        Timer1::TCCRB::clear();
        Timer1::TCCRC::clear();
        Timer1::TCNT ::clear();

        // Turn on CTC mode (Clear Timer on Compare Match)
        BitHandle<Timer1::TCCRB, WGM12>::set();
        // Set CS11 bit for Prescaler 8
        BitHandle<Timer1::TCCRB, CS11 >::set();
        }
    }

    static inline void flip_pin() {
        HAL::io_flip<Pin>();
    }

    static inline void set_compare(unsigned short ticks) {
        Timer1::OCRA::write(ticks);
    }

    static inline bool is_routine_active() {
        return BitHandle<Timer1::TIMSK, OCIE1A>::is_set();
    }

    static inline void arm_timer() {
        Timer1::TCNT::clear();                   // Start cleanly from tick 0
        W1CHandle<Timer1::TIFR, OCF1A>::clear(); // Clear stale interrupt flags
        BitHandle<Timer1::TIMSK, OCIE1A>::set(); // Enable interrupt
    }
 
    static inline void disable_interupt() {
        // Clear Timer Mask OCRA flag
        BitHandle<Timer1::TIMSK, OCIE1A>::clear();
    }
};

using dcc::DccSerializer;
using dcc::Command;
using dcc::Instruction;
using dcc::SpeedStep;
using dcc::Direction;
using dcc::DccScheduler;

using BBWriter = dcc::Bitbanger<
    Timer1Handle<platform::avr::GPIO_H::Pin<5>>,
    dcc::DefaultTimings
>;

auto bbwriter = BBWriter{};
auto scheduler = DccScheduler<BBWriter>{bbwriter};

// =====================

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
    Serial.begin(9600);
    bbwriter.configure();
    sei();
}

void loop() {
    uint32_t current_time = millis();

	if (current_time - previous_time >= delay_ms) {
		previous_time = current_time;
		update_scheduling();
	}

	scheduler.update();
}

ISR(TIMER1_COMPA_vect) { bbwriter.routine(); }