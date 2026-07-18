#pragma once
#include <Adafruit_SSD1306.h>
#include "Types.h"

namespace sentinel {
class DisplayManager {
 public:
  bool begin(TwoWire& wire);
  void update(uint32_t nowMs, const VoltageSnapshot& voltage, bool internetAvailable);
 private:
  Adafruit_SSD1306 display_{128, 64, &Wire, -1};
  bool ready_ = false;
  uint32_t lastUpdateMs_ = 0;
};
}  // namespace sentinel
