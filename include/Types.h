#pragma once

#include <Arduino.h>

namespace sentinel {
enum class PhaseCondition : uint8_t { Unknown, Normal, Lost, UnderVoltage, OverVoltage };
enum class EventCode : uint8_t {
  PhaseRLost, PhaseYLost, PhaseBLost, PhaseRRestored, PhaseYRestored, PhaseBRestored,
  UnderVoltage, UnderVoltageCleared, OverVoltage, OverVoltageCleared,
  Imbalance, ImbalanceCleared, InternetAvailable, InternetUnavailable
};

struct VoltageSnapshot {
  float phase[3] = {NAN, NAN, NAN};
  float imbalancePercent = NAN;
  PhaseCondition condition[3] = {PhaseCondition::Unknown, PhaseCondition::Unknown, PhaseCondition::Unknown};
  bool valid = false;
};

struct EnvironmentReading { float temperatureC = NAN; float humidityPercent = NAN; };
struct Event {
  EventCode code;
  float value = NAN;
  Event(EventCode code_ = EventCode::InternetAvailable, float value_ = NAN) : code(code_), value(value_) {}
};
struct EventBatch { Event events[8]; uint8_t count = 0; void add(EventCode code, float value = NAN) { if (count < 8) events[count++] = Event(code, value); } };
}  // namespace sentinel
