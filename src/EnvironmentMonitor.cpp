#include "EnvironmentMonitor.h"
#include "config.h"
namespace sentinel {
bool EnvironmentMonitor::begin(TwoWire& wire) {
  (void)wire;
  ready_ = sensor_.begin(0x44);
  return ready_;
}
void EnvironmentMonitor::update(uint32_t nowMs) {
  if (!ready_ || nowMs - lastReadMs_ < config::ENVIRONMENT_INTERVAL_MS) return;
  lastReadMs_ = nowMs; reading_.temperatureC = sensor_.readTemperature(); reading_.humidityPercent = sensor_.readHumidity();
}
}  // namespace sentinel
