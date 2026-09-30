#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "index_html.h"
#define LED_PIN 2           
#define BLINK_INTERVAL 500 

const char* ssid = "Yokogawna-Exfuflo-AQ1550";
const char* password = "12312312"; 
bool isWifiActive = false;
unsigned long lastBlinkTime = 0;
bool ledState = false;

WebServer server(80);

void handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}

void handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}

void updateStatusLed() {
    if (isWifiActive) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
            lastBlinkTime = currentMillis;
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
        }
    } else {
        digitalWrite(LED_PIN, HIGH); 
    }
}


void setup() {
    Serial.begin(460800);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    WiFi.mode(WIFI_AP);
    isWifiActive = WiFi.softAP(ssid, password);

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);

    server.begin();
}

void loop() {
    server.handleClient();
    updateStatusLed();
}