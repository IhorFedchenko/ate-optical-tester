#include "metrics.h"

TelemetryMetrics g_metrics;
static uint64_t totalRttUs = 0;

void metricsInit(void) {
    metricsReset();
}

void metricsReset(void) {
    g_metrics.packetsSent = 0;
    g_metrics.packetsRecv = 0;
    g_metrics.crcErrors = 0;
    g_metrics.linkQuality = 100.0f;
    g_metrics.packetLoss = 0.0f;
    g_metrics.rttMs = 0.0f;
    g_metrics.avgRttMs = 0.0f;
    g_metrics.maxRttMs = 0.0f;
    g_metrics.state = TEST_IDLE;
    g_metrics.elapsedTimeMs = 0;
    totalRttUs = 0;
}

void metricsStart(void) {
    metricsReset();
    g_metrics.state = TEST_RUNNING;
}

static void updateCumulativeStats(void) {
    if (g_metrics.packetsSent == 0) return;

    uint32_t totalLost = (g_metrics.packetsSent > g_metrics.packetsRecv) ? 
                         (g_metrics.packetsSent - g_metrics.packetsRecv) : 0;
    
    g_metrics.packetLoss = ((float)totalLost / g_metrics.packetsSent) * 100.0f;
    g_metrics.linkQuality = ((float)g_metrics.packetsRecv / g_metrics.packetsSent) * 100.0f;
}

void metricsOnPacketSent(void) {
    if (g_metrics.state != TEST_RUNNING) return;
    
    g_metrics.packetsSent++;
    g_metrics.elapsedTimeMs = g_metrics.packetsSent * 4;

    updateCumulativeStats();

    if (g_metrics.packetsSent >= TARGET_PACKETS) {
        g_metrics.state = TEST_FINISHED;
    }
}

void metricsOnPacketRecv(uint32_t rttUs) {
    if (g_metrics.state != TEST_RUNNING) return;

    g_metrics.packetsRecv++;
    g_metrics.rttMs = rttUs / 1000.0f;

    totalRttUs += rttUs;
    g_metrics.avgRttMs = (float)(totalRttUs / g_metrics.packetsRecv) / 1000.0f;

    if (g_metrics.rttMs > g_metrics.maxRttMs) {
        g_metrics.maxRttMs = g_metrics.rttMs;
    }

    updateCumulativeStats();
}

void metricsOnCrcError(void) {
    if (g_metrics.state != TEST_RUNNING) return;

    g_metrics.crcErrors++;
    updateCumulativeStats();
}