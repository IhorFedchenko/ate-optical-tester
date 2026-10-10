#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>

#define LED_PIN 2           
#define BLINK_INTERVAL 500
#define BLINK_INTERVAL_FAST 100

void statusLedInit();
void statusLedUpdate();
void statusLedSetFast(bool fast);

#endif // STATUS_LED_H