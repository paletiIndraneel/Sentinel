#include "EventDetector.h"
#include "config.h"

namespace sentinel {
static EventCode phaseEvent(uint8_t phase, bool restored) {
  return restored ? static_cast<EventCode>(static_cast<uint8_t>(EventCode::PhaseRRestored) + phase)
                  : static_cast<EventCode>(static_cast<uint8_t>(EventCode::PhaseRLost) + phase);
}
EventBatch EventDetector::evaluate(const VoltageSnapshot& reading) {
  EventBatch batch;
  if (!reading.valid) return batch;
  for (uint8_t i = 0; i < 3; ++i) {
    const PhaseCondition before = previous_[i], after = reading.condition[i];
    if (before == PhaseCondition::Unknown) {
      if (after == PhaseCondition::Lost) batch.add(phaseEvent(i, false), reading.phase[i]);
      if (after == PhaseCondition::UnderVoltage) batch.add(EventCode::UnderVoltage, reading.phase[i]);
      if (after == PhaseCondition::OverVoltage) batch.add(EventCode::OverVoltage, reading.phase[i]);
    } else if (before != after) {
      if (before == PhaseCondition::Lost) batch.add(phaseEvent(i, true), reading.phase[i]);
      if (after == PhaseCondition::Lost) batch.add(phaseEvent(i, false), reading.phase[i]);
      if (before == PhaseCondition::UnderVoltage) batch.add(EventCode::UnderVoltageCleared, reading.phase[i]);
      if (after == PhaseCondition::UnderVoltage) batch.add(EventCode::UnderVoltage, reading.phase[i]);
      if (before == PhaseCondition::OverVoltage) batch.add(EventCode::OverVoltageCleared, reading.phase[i]);
      if (after == PhaseCondition::OverVoltage) batch.add(EventCode::OverVoltage, reading.phase[i]);
    }
    previous_[i] = after;
  }
  const bool nowImbalanced = !isnan(reading.imbalancePercent) && reading.imbalancePercent > config::MAX_IMBALANCE_PERCENT;
  if (nowImbalanced != imbalanced_) batch.add(nowImbalanced ? EventCode::Imbalance : EventCode::ImbalanceCleared, reading.imbalancePercent);
  imbalanced_ = nowImbalanced;
  return batch;
}
}  // namespace sentinel
