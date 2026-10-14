#include <Arduino.h>
#include <examples/scheduler-example.hpp>

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