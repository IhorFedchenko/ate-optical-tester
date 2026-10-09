#include "crsf_service.h"
#include "metrics.h"

static unsigned long lastTxTime = 0;
static const unsigned long TX_INTERVAL_US = 4000; // 250 Hz (4 ms)

static uint8_t rxBuf[CRSF_FRAME_SIZE];
static uint8_t rxIndex = 0;
static unsigned long rxStartTime = 0;

static uint8_t crsfCrc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00;
    while (len--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0xD5;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void crsfServiceInit() {
    metricsInit();
    Serial1.begin(CRSF_BAUDRATE, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
    Serial2.begin(CRSF_BAUDRATE, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);
}

static void resetParser() {
  rxIndex = 0;
}

static void sendCrsfFrame() {
    uint8_t frame[CRSF_FRAME_SIZE];
    
    frame[0] = CRSF_SYNC_BYTE;  // 0xC8
    frame[1] = CRSF_FRAME_SIZE - 2; // 24 байти payload + CRC
    frame[2] = 0x16;            // RC Channels Frame

    // Наповнюємо payload
    for (int i = 3; i < CRSF_FRAME_SIZE - 1; i++) {
        frame[i] = (uint8_t)(g_metrics.packetsSent + i);
    }

    // Вставляємо розрахований CRC8 у останній байт
    frame[CRSF_FRAME_SIZE - 1] = crsfCrc8(&frame[2], CRSF_FRAME_SIZE - 3);

    Serial1.write(frame, CRSF_FRAME_SIZE);
    metricsOnPacketSent(); // Облік надісланого пакета
}

static void processIncomingByte(uint8_t b) {
    if (rxIndex == 0) {
        if (b == CRSF_SYNC_BYTE) {
            rxBuf[rxIndex++] = b;
            rxStartTime = micros(); // Фіксуємо час прибуття першого байта
        }
        return;
    }

    rxBuf[rxIndex++] = b;

    if (rxIndex >= CRSF_FRAME_SIZE) {
        uint8_t calculatedCrc = crsfCrc8(&rxBuf[2], CRSF_FRAME_SIZE - 3);
        uint8_t frameCrc = rxBuf[CRSF_FRAME_SIZE - 1];

        if (calculatedCrc == frameCrc) {
            uint32_t rttUs = micros() - rxStartTime;
            metricsOnPacketRecv(rttUs); // Валідний кадр + RTT
        } else {
            metricsOnCrcError(); // Битий кадр
        }

        rxIndex = 0;
    }
}

void crsfServiceLoop(void) {
    if (g_metrics.state != TEST_RUNNING) {
    while (Serial2.available() > 0) Serial2.read();
    resetParser();
    return;
  }

    unsigned long currentMicros = micros();

    // 2. Таймер відправки 250 Гц (4000 мкс)
    if (currentMicros - lastTxTime >= TX_INTERVAL_US) {
        lastTxTime = currentMicros;
        sendCrsfFrame();
    }

    // 3. Обробка вхідного потоку з перевіркою кадру
    while (Serial2.available() > 0) {
        processIncomingByte(Serial2.read());
    }
}