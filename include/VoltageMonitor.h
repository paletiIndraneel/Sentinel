#pragma once

#include <Adafruit_ADS1X15.h>
#include "Types.h"

namespace sentinel {
class VoltageMonitor {
 public:
  bool begin(TwoWire& wire);
  void update(uint32_t nowMs);
  bool hasNewSnapshot();
  VoltageSnapshot snapshot() const { return snapshot_; }

 private:
  Adafruit_ADS1115 ads_;
  double sum_[3] = {}, sumSquares_[3] = {};
  uint32_t samples_ = 0, windowStartedMs_ = 0, lastSampleMs_ = 0;
  bool ready_ = false, newSnapshot_ = false;
  VoltageSnapshot snapshot_;
};
}  // namespace sentinel
