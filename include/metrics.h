#ifndef METRICS_H
#define METRICS_H

#include <stdint.h>
#include <stdbool.h>

#define TARGET_PACKETS 3750U

enum TestState {
    TEST_IDLE = 0,
    TEST_RUNNING,
    TEST_FINISHED
};

enum Command : uint8_t {
  CMD_NONE = 0,
  CMD_TOGGLE,
  CMD_RESET
};


typedef struct {
    uint32_t packetsSent;
    uint32_t packetsRecv;
    uint32_t crcErrors;
    float linkQuality;
    float packetLoss;
    float rttMs;
    float avgRttMs;
    float maxRttMs;
    TestState state;
    uint32_t elapsedTimeMs;
} TelemetryMetrics;

extern TelemetryMetrics g_metrics;

void metricsInit(void);
void metricsReset(void);
void metricsStart(void);
void metricsOnPacketSent(void);
void metricsOnPacketRecv(uint32_t rttUs);
void metricsOnCrcError(void);
void metricsPostCommand(Command cmd);
void metricsProcessCommands(void);

#endif // METRICS_H