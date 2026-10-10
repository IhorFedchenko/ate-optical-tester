#include "status_led.h"

static unsigned long lastBlinkTime = 0;
static unsigned blinkInterval = BLINK_INTERVAL;

static bool ledState = false;

void statusLedInit() {
    pinMode(LED_PIN, OUTPUT);
    ledState = true;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    lastBlinkTime = millis();
}

void statusLedSetFast(bool fast){
   blinkInterval = fast ? BLINK_INTERVAL_FAST : BLINK_INTERVAL;
}

void statusLedUpdate() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkTime >= blinkInterval) {
        lastBlinkTime = currentMillis;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
}