#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

// Ініціалізація SoftAP та асинхронних маршрутів API
void initWebServer(const char* ssid, const char* password);

#endif