#ifndef CRSF_SERVICE_H
#define CRSF_SERVICE_H

#include <Arduino.h>
#include "metrics.h"

#define UART1_RX_PIN 16
#define UART1_TX_PIN 17

#define UART2_RX_PIN 23
#define UART2_TX_PIN 22

#define CRSF_FRAME_SIZE 26
#define CRSF_SYNC_BYTE  0xC8

void crsfServiceInit();
void crsfServiceLoop();

#endif