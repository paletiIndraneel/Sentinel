#pragma once
#include <SD.h>
#include "Types.h"

namespace sentinel {
class Logger {
 public:
  bool begin(SPIClass& spi, uint8_t chipSelect);
  void logEvent(const String& timestamp, const Event& event);
  void logReading(const String& timestamp, const VoltageSnapshot& voltage, const EnvironmentReading& environment, bool internetAvailable);
 private:
  bool ready_ = false;
  void append(const char* path, const String& line);
};
const char* eventName(EventCode code);
}  // namespace sentinel
