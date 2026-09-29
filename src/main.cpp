#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "index_html.h"

const char* ssid = "Yokogawna-Exfuflo-AQ1550";
const char* password = "12312312"; 

WebServer server(80);

void handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}

void handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}


void setup() {
    Serial.begin(460800);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);

    server.begin();
}

void loop() {
    server.handleClient();
}