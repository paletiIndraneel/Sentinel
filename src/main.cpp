#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "config.h"
#include "DisplayManager.h"
#include "EnvironmentMonitor.h"
#include "EventDetector.h"
#include "InternetMonitor.h"
#include "Logger.h"
#include "RTCManager.h"
#include "VoltageMonitor.h"

namespace sentinel {
class SentinelApp {
 public:
  void begin();
  void update();
 private:
  void logEvents(const EventBatch& events);
  VoltageMonitor voltage_;
  EventDetector events_;
  InternetMonitor internet_;
  RTCManager rtc_;
  Logger logger_;
  DisplayManager display_;
  EnvironmentMonitor environment_;
  VoltageSnapshot latestVoltage_;
  uint32_t lastReadingLogMs_ = 0;
};

void SentinelApp::begin() {
  Serial.begin(115200);
  pinMode(config::pin::STATUS_LED, OUTPUT); pinMode(config::pin::WIFI_LED, OUTPUT);
  digitalWrite(config::pin::STATUS_LED, LOW); digitalWrite(config::pin::WIFI_LED, LOW);
  Wire.begin(config::pin::I2C_SDA, config::pin::I2C_SCL);
  SPI.begin(config::pin::SD_SCK, config::pin::SD_MISO, config::pin::SD_MOSI, config::pin::SD_CS);
  Serial.printf("Sentinel v1.0 boot: ADS=%d RTC=%d OLED=%d SHT31=%d SD=%d\n", voltage_.begin(Wire), rtc_.begin(Wire), display_.begin(Wire), environment_.begin(Wire), logger_.begin(SPI, config::pin::SD_CS));
  internet_.begin();
}
void SentinelApp::logEvents(const EventBatch& events) { for (uint8_t i = 0; i < events.count; ++i) logger_.logEvent(rtc_.timestamp(), events.events[i]); }
void SentinelApp::update() {
  const uint32_t now = millis();
  internet_.update(now);
  digitalWrite(config::pin::WIFI_LED, internet_.available() ? HIGH : LOW);
  if (internet_.takeStatusChanged()) logger_.logEvent(rtc_.timestamp(), {internet_.available() ? EventCode::InternetAvailable : EventCode::InternetUnavailable});
  voltage_.update(now); environment_.update(now);
  if (voltage_.hasNewSnapshot()) { latestVoltage_ = voltage_.snapshot(); logEvents(events_.evaluate(latestVoltage_)); digitalWrite(config::pin::STATUS_LED, !digitalRead(config::pin::STATUS_LED)); }
  display_.update(now, latestVoltage_, internet_.available());
  if (latestVoltage_.valid && now - lastReadingLogMs_ >= config::READING_LOG_INTERVAL_MS) { lastReadingLogMs_ = now; logger_.logReading(rtc_.timestamp(), latestVoltage_, environment_.reading(), internet_.available()); }
}
}  // namespace sentinel

sentinel::SentinelApp app;
void setup() { app.begin(); }
void loop() { app.update(); }
