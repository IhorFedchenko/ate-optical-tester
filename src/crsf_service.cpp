#include "crsf_service.h"

bool isTestRunning = false;
volatile uint32_t packetsSent = 0;
volatile uint32_t packetsRecv = 0;

static unsigned long lastTxTime = 0;
static const unsigned long TX_INTERVAL_US = 4000; // 250 Hz (4 ms)

uint8_t testPayload[26] = {
    0xC8, 0x18, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void crsfServiceInit() {
    Serial1.begin(CRSF_BAUDRATE, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
    Serial2.begin(CRSF_BAUDRATE, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);
}

void crsfServiceLoop() {
    if (isTestRunning) {
        if (micros() - lastTxTime >= TX_INTERVAL_US) {
            lastTxTime = micros();
            Serial1.write(testPayload, sizeof(testPayload));
            packetsSent++;
        }
    }

    static uint8_t rxIndex = 0;

    while (Serial2.available() > 0) {
        Serial2.read();
        rxIndex++;
        if (rxIndex >= 26) {
            packetsRecv++;
            rxIndex = 0;
        }
    }
}