#include "web_server.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index_html.h"
#include "metrics.h" // 1. Підключаємо модуль метрик

static AsyncWebServer server(80);

void initWebServer(const char* ssid, const char* password) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", INDEX_HTML);
    });

    // 2. Повертаємо повний JSON з усіма метриками
    server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
        char jsonBuffer[256];
        snprintf(jsonBuffer, sizeof(jsonBuffer), 
                 "{\"sent\":%u,\"recv\":%u,\"crc\":%u,\"lq\":%.1f,\"loss\":%.2f,\"rtt\":%.1f,\"avg_rtt\":%.1f,\"state\":%d,\"elapsed\":%u}", 
                 g_metrics.packetsSent, g_metrics.packetsRecv, g_metrics.crcErrors, 
                 g_metrics.linkQuality, g_metrics.packetLoss, g_metrics.rttMs, 
                 g_metrics.avgRttMs, (int)g_metrics.state, g_metrics.elapsedTimeMs);

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonBuffer);
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
    });

    // 3. Управління станом тесту
    server.on("/api/toggle", HTTP_POST, [](AsyncWebServerRequest *request){
        metricsPostCommand(CMD_TOGGLE);
        request->send(200, "text/plain", "ok");
    });

    // 4. Скидання статистики
    server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *request){
        metricsPostCommand(CMD_RESET);
        request->send(200, "text/plain", "reset_ok");
    });

    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "404: Not Found");
    });

    server.begin();
}