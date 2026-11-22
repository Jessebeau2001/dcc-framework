

using LEDPin = HAL::LED_ONBOARD;

void setup() {
    HAL::io_configure<LEDPin>(HAL::IOMode::Output);
}

void loop() {
    HAL::io_flip<LEDPin>();
    delay(2000);
    HAL::io_flip<LEDPin>();
    delay(2000);
}