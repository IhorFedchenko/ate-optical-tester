#include "status_led.h"

static unsigned long lastBlinkTime = 0;
static bool ledState = false;

void statusLedInit() {
    pinMode(LED_PIN, OUTPUT);
    ledState = true;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    lastBlinkTime = millis();
}

void statusLedUpdate() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
        lastBlinkTime = currentMillis;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
}