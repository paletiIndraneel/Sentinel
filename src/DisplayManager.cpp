#include "DisplayManager.h"
#include "config.h"

namespace sentinel {
bool DisplayManager::begin(TwoWire& wire) {
  (void)wire;
  ready_ = display_.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (ready_) { display_.clearDisplay(); display_.setTextColor(SSD1306_WHITE); display_.setTextSize(1); display_.setCursor(0, 0); display_.println("Sentinel v1.0"); display_.display(); }
  return ready_;
}
void DisplayManager::update(uint32_t nowMs, const VoltageSnapshot& voltage, bool internetAvailable) {
  if (!ready_ || nowMs - lastUpdateMs_ < config::DISPLAY_INTERVAL_MS) return;
  lastUpdateMs_ = nowMs; display_.clearDisplay(); display_.setTextSize(1); display_.setCursor(0, 0);
  display_.printf("R: %6.1f V\nY: %6.1f V\nB: %6.1f V\n", voltage.phase[0], voltage.phase[1], voltage.phase[2]);
  display_.printf("NET: %s\n", internetAvailable ? "ONLINE" : "OFFLINE");
  if (!voltage.valid) display_.print("Voltage: acquiring...");
  else if (voltage.condition[0] != PhaseCondition::Normal) display_.print("FAULT: phase R");
  else if (voltage.condition[1] != PhaseCondition::Normal) display_.print("FAULT: phase Y");
  else if (voltage.condition[2] != PhaseCondition::Normal) display_.print("FAULT: phase B");
  else if (!isnan(voltage.imbalancePercent) && voltage.imbalancePercent > config::MAX_IMBALANCE_PERCENT) display_.printf("IMBALANCE %.1f%%", voltage.imbalancePercent);
  else display_.print("Voltage: normal");
  display_.display();
}
}  // namespace sentinel
