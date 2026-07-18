#pragma once
#include "Types.h"

namespace sentinel {
class EventDetector {
 public:
  EventBatch evaluate(const VoltageSnapshot& reading);
 private:
  PhaseCondition previous_[3] = {PhaseCondition::Unknown, PhaseCondition::Unknown, PhaseCondition::Unknown};
  bool imbalanced_ = false;
};
}  // namespace sentinel
