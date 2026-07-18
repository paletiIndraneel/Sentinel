#pragma once
#include <RTClib.h>

namespace sentinel {
class RTCManager {
 public:
  bool begin(TwoWire& wire);
  String timestamp();
 private:
  RTC_DS3231 rtc_;
  bool ready_ = false;
};
}  // namespace sentinel
