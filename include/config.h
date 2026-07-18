#pragma once

#include <Arduino.h>

namespace sentinel::config {
// Keep credentials out of source control in a private config override.
#if __has_include("config_private.h")
#include "config_private.h"
#else
constexpr char WIFI_SSID[] = "CHANGE_ME";
constexpr char WIFI_PASSWORD[] = "CHANGE_ME";
#endif

// Public HTTP endpoint used only to verify internet reachability. It bypasses
// DNS deliberately, so a successful result proves routing beyond the LAN.
const IPAddress INTERNET_TEST_IP(1, 1, 1, 1);
constexpr char INTERNET_TEST_HOST[] = "one.one.one.one";
constexpr uint16_t INTERNET_TEST_PORT = 80;

namespace pin {
constexpr uint8_t STATUS_LED = 4;
constexpr uint8_t WIFI_LED = 5;
constexpr uint8_t I2C_SDA = 8;
constexpr uint8_t I2C_SCL = 9;
constexpr uint8_t SD_CS = 10;
constexpr uint8_t SD_MOSI = 11;
constexpr uint8_t SD_SCK = 12;
constexpr uint8_t SD_MISO = 13;
}  // namespace pin

constexpr uint32_t VOLTAGE_SAMPLE_INTERVAL_MS = 4;
constexpr uint32_t VOLTAGE_WINDOW_MS = 1000;
constexpr uint32_t READING_LOG_INTERVAL_MS = 60000;
constexpr uint32_t DISPLAY_INTERVAL_MS = 500;
constexpr uint32_t ENVIRONMENT_INTERVAL_MS = 5000;
constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
constexpr uint32_t WIFI_RETRY_INTERVAL_MS = 30000;
constexpr uint32_t INTERNET_TEST_INTERVAL_MS = 60000;
constexpr uint32_t INTERNET_TEST_TIMEOUT_MS = 7000;

// Calibrate each channel with an isolated, trusted reference meter before use.
constexpr float PHASE_SCALE[] = {1.0F, 1.0F, 1.0F};
constexpr float PHASE_LOSS_V = 80.0F;
constexpr float UNDER_VOLTAGE_V = 180.0F;
constexpr float OVER_VOLTAGE_V = 260.0F;
constexpr float MAX_IMBALANCE_PERCENT = 5.0F;
}  // namespace sentinel::config
