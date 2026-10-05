#include "web_server.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index_html.h"

static AsyncWebServer server(80);

void initWebServer(const char* ssid, const char* password) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    // Головна сторінка
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", INDEX_HTML);
    });

    // Отримання телеметрії (JSON)
    server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
        char jsonBuffer[128];
        snprintf(jsonBuffer, sizeof(jsonBuffer), 
                 "{\"sent\":%u,\"recv\":%u,\"running\":%s}", 
                 packetsSent, packetsRecv, isTestRunning ? "true" : "false");

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonBuffer);
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
    });

    // Старт / Стоп тесту
    server.on("/api/toggle", HTTP_POST, [](AsyncWebServerRequest *request){
        isTestRunning = !isTestRunning;
        request->send(200, "text/plain", isTestRunning ? "started" : "stopped");
    });

    // Скидання лічильників
    server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *request){
        packetsSent = 0;
        packetsRecv = 0;
        request->send(200, "text/plain", "reset_ok");
    });

    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "404: Not Found");
    });

    server.begin();
}