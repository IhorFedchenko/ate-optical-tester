#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Глобальні прапорці та лічильники для WebUI
extern bool isTestRunning;
extern volatile uint32_t packetsSent;
extern volatile uint32_t packetsRecv;

// Ініціалізація SoftAP та асинхронних маршрутів API
void initWebServer(const char* ssid, const char* password);

#endif