#pragma once
#include <WiFi.h>
#include <AsyncTCP.h>
#include <functional>

namespace sentinel {
  class InternetMonitor {
  public:
    bool isWifiConnected() const;
    bool takeWifiStatusChanged();
    bool isInternetAvailable() const;
    void begin();
    void update(uint32_t nowMs);
    bool available() const { return internetAvailable_; }
    bool takeStatusChanged();
    using DateCallback = std::function<void(const String&)>;
    void setDateCallback(DateCallback cb);
  private:
    bool wifiConnected_ = false;
    bool wifiChanged_ = false;
    DateCallback dateCallback_;
    static void onConnect(void* arg, AsyncClient* client);
    static void onData(void* arg, AsyncClient* client, void* data, size_t len);
    static void onDisconnect(void* arg, AsyncClient* client);
    static void onError(void* arg, AsyncClient* client, int8_t error);
    void beginProbe(uint32_t nowMs);
    void setInternetAvailable(bool value);
    AsyncClient client_;
    bool connecting_ = false, probeRunning_ = false, internetAvailable_ = false, changed_ = false, hadResult_ = false;
    uint32_t connectStartedMs_ = 0, lastWifiAttemptMs_ = 0, probeStartedMs_ = 0, lastProbeMs_ = 0;
  };
}  // namespace sentinel
