#ifndef CRSF_SERVICE_H
#define CRSF_SERVIVE_H

#include <Arduino.h>

#define UART1_RX_PIN 16
#define UART1_TX_PIN 17

#define UART2_RX_PIN 23
#define UART2_TX_PIN 22

extern bool isTestRunning;
extern volatile uint32_t packetsSent;
extern volatile uint32_t packetRecv;

extern uint8_t testPayload[26];

void crsfServiceInit();
void crsfServiceLoop();

#endif