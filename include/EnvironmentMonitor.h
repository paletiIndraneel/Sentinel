#pragma once
#include <Adafruit_SHT31.h>
#include "Types.h"

namespace sentinel {
class EnvironmentMonitor {
 public:
  bool begin(TwoWire& wire);
  void update(uint32_t nowMs);
  EnvironmentReading reading() const { return reading_; }
 private:
  Adafruit_SHT31 sensor_;
  EnvironmentReading reading_;
  bool ready_ = false;
  uint32_t lastReadMs_ = 0;
};
}  // namespace sentinel
