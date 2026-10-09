#include <Arduino.h>
#include "web_server.h"
#include "crsf_service.h"
#include "status_led.h"
#include "metrics.h"

const char* ssid = "Yokogawna-Exfuflo-AQ1550";
const char* password = "12312312"; 

void setup() {
    Serial.begin(DEBUG_BAUTRATE);

    statusLedInit();
    crsfServiceInit();
    initWebServer(ssid, password);
}

void loop() {
    metricsProcessCommands();
    statusLedUpdate();
    crsfServiceLoop();
}