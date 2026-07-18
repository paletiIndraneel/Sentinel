#include "VoltageMonitor.h"
#include "config.h"
#include <math.h>

namespace sentinel {
bool VoltageMonitor::begin(TwoWire& wire) {
  ready_ = ads_.begin(0x48, &wire);
  if (ready_) { ads_.setGain(GAIN_ONE); ads_.setDataRate(RATE_ADS1115_860SPS); }
  return ready_;
}

void VoltageMonitor::update(uint32_t nowMs) {
  if (!ready_ || nowMs - lastSampleMs_ < config::VOLTAGE_SAMPLE_INTERVAL_MS) return;
  lastSampleMs_ = nowMs;
  if (windowStartedMs_ == 0) windowStartedMs_ = nowMs;
  for (uint8_t channel = 0; channel < 3; ++channel) {
    const float volts = ads_.computeVolts(ads_.readADC_SingleEnded(channel));
    sum_[channel] += volts;
    sumSquares_[channel] += volts * volts;
  }
  ++samples_;
  if (nowMs - windowStartedMs_ < config::VOLTAGE_WINDOW_MS || samples_ < 20) return;

  float average = 0.0F;
  bool allPresent = true;
  for (uint8_t i = 0; i < 3; ++i) {
    const double mean = sum_[i] / samples_;
    const double variance = max(0.0, sumSquares_[i] / samples_ - mean * mean);
    snapshot_.phase[i] = sqrt(variance) * config::PHASE_SCALE[i];
    snapshot_.condition[i] = snapshot_.phase[i] < config::PHASE_LOSS_V ? PhaseCondition::Lost :
                             snapshot_.phase[i] < config::UNDER_VOLTAGE_V ? PhaseCondition::UnderVoltage :
                             snapshot_.phase[i] > config::OVER_VOLTAGE_V ? PhaseCondition::OverVoltage : PhaseCondition::Normal;
    average += snapshot_.phase[i];
    allPresent &= snapshot_.condition[i] != PhaseCondition::Lost;
    sum_[i] = sumSquares_[i] = 0;
  }
  average /= 3.0F;
  float maxDeviation = 0.0F;
  for (float voltage : snapshot_.phase) maxDeviation = max(maxDeviation, fabsf(voltage - average));
  snapshot_.imbalancePercent = (allPresent && average > 0.1F) ? maxDeviation / average * 100.0F : NAN;
  snapshot_.valid = true;
  samples_ = 0; windowStartedMs_ = nowMs; newSnapshot_ = true;
}

bool VoltageMonitor::hasNewSnapshot() { const bool value = newSnapshot_; newSnapshot_ = false; return value; }
}  // namespace sentinel
