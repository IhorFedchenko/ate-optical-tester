#include <Arduino.h>
#include "web_server.h"
#include "crsf_service.h"

#define LED_PIN 2           
#define BLINK_INTERVAL 500

const char* ssid = "Yokogawna-Exfuflo-AQ1550";
const char* password = "12312312"; 

unsigned long lastBlinkTime = 0;
bool ledState = false;

void updateStatusLed() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
        lastBlinkTime = currentMillis;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }
}

void setup() {
    Serial.begin(460800);

    crsfServiceInit();

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    initWebServer(ssid, password);
}

void loop() {
    updateStatusLed();
    crsfServiceLoop();
}