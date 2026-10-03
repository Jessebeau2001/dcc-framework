#include <Arduino.h>
#include "HAL/bitbang.h"

void setup() {
    bitbang::init();
    // Serial.begin(9600);
}

constexpr uint8_t FRAME[2] = {50U, 50U}; 

void loop() {
    if (bitbang::available_for_write() > 8) {
        bitbang::write_frame(FRAME, 2);
        // Serial.println("Frame written");
    }
}