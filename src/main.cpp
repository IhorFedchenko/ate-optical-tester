#include <Arduino.h>
#include "web_server.h"

#define LED_PIN 2           
#define BLINK_INTERVAL 500

#define UART1_RX_PIN 16
#define UART1_TX_PIN 17

#define UART2_RX_PIN 23
#define UART2_TX_PIN 22

const char* ssid = "Yokogawna-Exfuflo-AQ1550";
const char* password = "12312312"; 

bool isTestRunning = false;
volatile uint32_t packetsSent = 0;
volatile uint32_t packetsRecv = 0;

unsigned long lastTxTime = 0;
const unsigned long TX_INTERVAL_US = 4000; // 250 Hz (4 ms)

unsigned long lastBlinkTime = 0;
bool ledState = false;

// Тестовий payload CRSF (26 байт)
uint8_t testPayload[26] = {
    0xC8, 0x18, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

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

    Serial1.begin(CRSF_BAUDRATE, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
    Serial2.begin(CRSF_BAUDRATE, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    initWebServer(ssid, password);
}

void loop() {
    updateStatusLed();

    // 1. Відправка кадрів у Serial1 (250 Гц)
    if (isTestRunning) {
        if (micros() - lastTxTime >= TX_INTERVAL_US) {
            lastTxTime = micros();
            Serial1.write(testPayload, sizeof(testPayload));
            packetsSent++;
        }
    }

    // 2. Зчитування байтів з Serial2 (Physical Loopback)
    static uint8_t rxIndex = 0;

    while (Serial2.available() > 0) {
        Serial2.read();
        rxIndex++;
        if (rxIndex >= 26){
            packetsRecv++;
            rxIndex = 0;
        }     
    }
}