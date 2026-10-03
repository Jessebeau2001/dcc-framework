#include <Arduino.h>
#include "HAL/bitbang.h"

void setup() {
    bitbang::init();
    // Serial.begin(9600);
}

constexpr uint8_t FRAME[8] = {50U, 50U, 50U, 50U, 50U, 50U, 50U, 50U }; 

void loop() {
    if (bitbang::available_for_write() > 8) {
        bitbang::write_frame(FRAME, 2);
    }
}

// #include "measure.hpp"