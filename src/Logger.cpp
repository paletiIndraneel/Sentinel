#include "Logger.h"

namespace sentinel {
const char* eventName(EventCode code) {
  static constexpr const char* names[] = {"PHASE_R_LOST", "PHASE_Y_LOST", "PHASE_B_LOST", "PHASE_R_RESTORED", "PHASE_Y_RESTORED", "PHASE_B_RESTORED", "UNDER_VOLTAGE", "UNDER_VOLTAGE_CLEARED", "OVER_VOLTAGE", "OVER_VOLTAGE_CLEARED", "VOLTAGE_IMBALANCE", "VOLTAGE_IMBALANCE_CLEARED", "INTERNET_AVAILABLE", "INTERNET_UNAVAILABLE"};
  return names[static_cast<uint8_t>(code)];
}
bool Logger::begin(SPIClass& spi, uint8_t chipSelect) {
  ready_ = SD.begin(chipSelect, spi);
  if (!ready_) return false;
  if (!SD.exists("/events.csv")) append("/events.csv", "timestamp,event,value");
  if (!SD.exists("/readings.csv")) append("/readings.csv", "timestamp,phase_r_vrms,phase_y_vrms,phase_b_vrms,imbalance_pct,temperature_c,humidity_pct,internet_available");
  return true;
}
void Logger::append(const char* path, const String& line) {
  if (!ready_) return;
  File file = SD.open(path, FILE_APPEND);
  if (file) { file.println(line); file.close(); }
}
void Logger::logEvent(const String& timestamp, const Event& event) {
  const String row = timestamp + ',' + eventName(event.code) + ',' + String(event.value, 2);
  Serial.println(row); append("/events.csv", row);
}
void Logger::logReading(const String& timestamp, const VoltageSnapshot& voltage, const EnvironmentReading& environment, bool internetAvailable) {
  const String row = timestamp + ',' + String(voltage.phase[0], 2) + ',' + String(voltage.phase[1], 2) + ',' + String(voltage.phase[2], 2) + ',' + String(voltage.imbalancePercent, 2) + ',' + String(environment.temperatureC, 2) + ',' + String(environment.humidityPercent, 2) + ',' + (internetAvailable ? "1" : "0");
  append("/readings.csv", row);
}
}  // namespace sentinel
