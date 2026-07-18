#include "InternetMonitor.h"
#include "config.h"
#include <cstring>

namespace sentinel {
void InternetMonitor::begin() {
  WiFi.mode(WIFI_STA);
  client_.onConnect(onConnect, this); client_.onData(onData, this);
  client_.onDisconnect(onDisconnect, this); client_.onError(onError, this);
  lastWifiAttemptMs_ = millis() - config::WIFI_RETRY_INTERVAL_MS;
  lastProbeMs_ = millis() - config::INTERNET_TEST_INTERVAL_MS;
}
void InternetMonitor::update(uint32_t nowMs) {
  if (strlen(config::WIFI_SSID) == 0) return;
  if (WiFi.status() != WL_CONNECTED) {
    if (!connecting_ && nowMs - lastWifiAttemptMs_ >= config::WIFI_RETRY_INTERVAL_MS) {
      WiFi.begin(config::WIFI_SSID, config::WIFI_PASSWORD);
      connecting_ = true; connectStartedMs_ = nowMs; lastWifiAttemptMs_ = nowMs;
    }
    if (connecting_ && nowMs - connectStartedMs_ >= config::WIFI_CONNECT_TIMEOUT_MS) { WiFi.disconnect(); connecting_ = false; }
    if (probeRunning_) { client_.close(); probeRunning_ = false; }
    if (hadResult_) setInternetAvailable(false);
    return;
  }
  connecting_ = false;
  if (!probeRunning_ && nowMs - lastProbeMs_ >= config::INTERNET_TEST_INTERVAL_MS) beginProbe(nowMs);
  if (probeRunning_ && nowMs - probeStartedMs_ >= config::INTERNET_TEST_TIMEOUT_MS) { client_.close(); probeRunning_ = false; lastProbeMs_ = nowMs; setInternetAvailable(false); }
}
void InternetMonitor::beginProbe(uint32_t nowMs) {
  probeRunning_ = client_.connect(config::INTERNET_TEST_IP, config::INTERNET_TEST_PORT);
  probeStartedMs_ = nowMs; lastProbeMs_ = nowMs;
  if (!probeRunning_) setInternetAvailable(false);
}
void InternetMonitor::onConnect(void* arg, AsyncClient* client) {
  auto* self = static_cast<InternetMonitor*>(arg);
  const String request = String("GET /cdn-cgi/trace HTTP/1.1\r\nHost: ") + config::INTERNET_TEST_HOST + "\r\nConnection: close\r\n\r\n";
  client->write(request.c_str()); self->probeRunning_ = true;
}
void InternetMonitor::onData(void* arg, AsyncClient* client, void* data, size_t len) {
  auto* self = static_cast<InternetMonitor*>(arg); const char* bytes = static_cast<const char*>(data);
  const bool success = len >= 12 && strncmp(bytes, "HTTP/1.1 2", 10) == 0;
  self->setInternetAvailable(success); self->probeRunning_ = false; client->close();
}
void InternetMonitor::onDisconnect(void* arg, AsyncClient*) { auto* self = static_cast<InternetMonitor*>(arg); if (self->probeRunning_) { self->probeRunning_ = false; self->setInternetAvailable(false); } }
void InternetMonitor::onError(void* arg, AsyncClient*, int8_t) { auto* self = static_cast<InternetMonitor*>(arg); self->probeRunning_ = false; self->setInternetAvailable(false); }
void InternetMonitor::setInternetAvailable(bool value) {
  if (!hadResult_ || internetAvailable_ != value) { internetAvailable_ = value; changed_ = true; }
  hadResult_ = true;
}
bool InternetMonitor::takeStatusChanged() { const bool value = changed_; changed_ = false; return value; }
}  // namespace sentinel
