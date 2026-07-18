#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>

#include "config.h"
#include "DisplayManager.h"
#include "EnvironmentMonitor.h"
#include "EventDetector.h"
#include "InternetMonitor.h"
#include "TimeManager.h"
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
  void updateTrafficLights();

  VoltageMonitor voltage_;
  EventDetector events_;
  InternetMonitor internet_;
  TimeManager time_;
  RTCManager rtc_;
  Logger logger_;
  DisplayManager display_;
  EnvironmentMonitor environment_;

  VoltageSnapshot latestVoltage_;
  uint32_t lastReadingLogMs_ = 0;
  int8_t trafficState_ = -1;
};

void SentinelApp::begin() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Boot 1");

  Wire.begin(config::pin::I2C_SDA, config::pin::I2C_SCL);
  time_.begin();
  pinMode(config::pin::TRAFFIC_RED, OUTPUT);
  pinMode(config::pin::TRAFFIC_YELLOW, OUTPUT);
  pinMode(config::pin::TRAFFIC_GREEN, OUTPUT);
  pinMode(config::pin::WIFI_LED, OUTPUT);
  pinMode(config::pin::STATUS_LED, OUTPUT);
  Serial.print("[Time] ");
  Serial.println(time_.getTimestamp());
  Serial.println("Boot 2");

  // Disabled modules
  // voltage_.begin(Wire);
  // rtc_.begin(Wire);
  // display_.begin(Wire);
  // environment_.begin(Wire);
  // logger_.begin(SPI, config::pin::SD_CS);

  Serial.println("Boot 3");

  internet_.begin();

  Serial.println("Boot Complete");
}

void SentinelApp::logEvents(const EventBatch& events) {
  // Logger disabled
}

void SentinelApp::updateTrafficLights() {
  // Red: No Wi-Fi
  // Yellow: Wi-Fi connected but Internet unavailable
  // Green: Internet available

  const int8_t state =
      internet_.available() ? 2 :
      (WiFi.status() == WL_CONNECTED ? 1 : 0);

  if (state == trafficState_)
    return;

  trafficState_ = state;

  digitalWrite(config::pin::TRAFFIC_RED, state == 0 ? HIGH : LOW);
  digitalWrite(config::pin::TRAFFIC_YELLOW, state == 1 ? HIGH : LOW);
  digitalWrite(config::pin::TRAFFIC_GREEN, state == 2 ? HIGH : LOW);

  Serial.printf(
      "[Traffic] %s (WiFi=%d Internet=%s)\n",
      state == 0 ? "RED" :
      state == 1 ? "YELLOW" :
                   "GREEN",
      WiFi.status(),
      internet_.available() ? "ONLINE" : "OFFLINE");
}

void SentinelApp::update() {
  const uint32_t now = millis();

  // Internet monitor
  internet_.update(now);

  // Wi-Fi LED
  digitalWrite(
      config::pin::WIFI_LED,
      internet_.available() ? HIGH : LOW);

  // Traffic LEDs
  updateTrafficLights();

  // Everything below is disabled for Sentinel v1.0

  /*
  if (internet_.takeStatusChanged())
      logger_.logEvent(
          rtc_.timestamp(),
          {internet_.available()
               ? EventCode::InternetAvailable
               : EventCode::InternetUnavailable});

  voltage_.update(now);
  environment_.update(now);

  if (voltage_.hasNewSnapshot()) {
      latestVoltage_ = voltage_.snapshot();

      logEvents(events_.evaluate(latestVoltage_));

      digitalWrite(
          config::pin::STATUS_LED,
          !digitalRead(config::pin::STATUS_LED));
  }

  display_.update(
      now,
      latestVoltage_,
      internet_.available());

  if (latestVoltage_.valid &&
      now - lastReadingLogMs_ >=
          config::READING_LOG_INTERVAL_MS) {

      lastReadingLogMs_ = now;

      logger_.logReading(
          rtc_.timestamp(),
          latestVoltage_,
          environment_.reading(),
          internet_.available());
  }
  */
}

}  // namespace sentinel

sentinel::SentinelApp app;

void setup() {
  app.begin();
}

void loop() {
  app.update();
}