#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>

#define LED_PIN 2           
#define BLINK_INTERVAL 500

void statusLedInit();
void statusLedUpdate();

#endif // STATUS_LED_H