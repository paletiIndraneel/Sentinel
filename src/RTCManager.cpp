#include "RTCManager.h"
namespace sentinel {
bool RTCManager::begin(TwoWire& wire) {
  ready_ = rtc_.begin(&wire);
  // Compile time is a fallback only; set the RTC accurately during commissioning.
  if (ready_ && rtc_.lostPower()) rtc_.adjust(DateTime(F(__DATE__), F(__TIME__)));
  return ready_;
}
String RTCManager::timestamp() {
  if (!ready_) return String("uptime-") + millis();
  const DateTime time = rtc_.now(); char text[25];
  snprintf(text, sizeof(text), "%04d-%02d-%02dT%02d:%02d:%02d", time.year(), time.month(), time.day(), time.hour(), time.minute(), time.second());
  return text;
}
}  // namespace sentinel
