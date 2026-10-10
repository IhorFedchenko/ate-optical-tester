#include "web_server.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index_html.h"
#include "metrics.h"
#include "status_led.h"

static AsyncWebServer server(80);

static const uint8_t AP_FIRST_CHANNEL = 1;
static const uint8_t AP_LAST_CHANNEL = 11;
static const uint8_t AP_FALLBACK_CHANNEL = 6;
static const uint32_t SCAN_MS_PER_CHANNEL = 300;
static const unsigned long SCAN_TIMEOUT_MS = 10000;


static uint8_t pickLeastCrowdedChannel() {
  statusLedSetFast(true);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  unsigned long t0 = millis();
  WiFi.scanNetworks(true, true, false, SCAN_MS_PER_CHANNEL);

  int n;
  while ((n = WiFi.scanComplete()) == WIFI_SCAN_RUNNING &&
         millis() - t0 < SCAN_TIMEOUT_MS) {
    statusLedUpdate();
    delay(10);
  }

  statusLedSetFast(false);
  Serial.printf("[WiFi] scan took %lu ms, networks found: %d\n", millis() - t0, n);

  if (n < 0) {
    WiFi.scanDelete();
    return AP_FALLBACK_CHANNEL;
  }

  uint8_t best = AP_FALLBACK_CHANNEL;
  uint32_t bestScore = UINT32_MAX;

  for (uint8_t c = AP_FIRST_CHANNEL; c <= AP_LAST_CHANNEL; c++) {
    uint32_t score = 0;
    for (int i = 0; i < n; i++) {
      int d = abs((int)WiFi.channel(i) - (int)c);
      if (d >= 5) continue;
      int power = 100 + WiFi.RSSI(i);
      if (power < 0) power = 0;
      score += (uint32_t)(5 - d) * power;
    }
    Serial.printf("[WiFi] channel %u: interference %u\n", c, (unsigned)score);
    if (score < bestScore) {
      bestScore = score;
      best = c;
    }
  }

  WiFi.scanDelete();
  return best;
}

void initWebServer(const char* ssid, const char* password) {
  uint8_t channel = pickLeastCrowdedChannel();
  Serial.printf("[WiFi] AP channel: %u\n", channel);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, channel);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", INDEX_HTML);
  });

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

  server.on("/api/toggle", HTTP_POST, [](AsyncWebServerRequest *request){
    metricsPostCommand(CMD_TOGGLE);
    request->send(200, "text/plain", "ok");
  });

  server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *request){
    metricsPostCommand(CMD_RESET);
    request->send(200, "text/plain", "reset_ok");
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not Found");
  });

  server.begin();
}