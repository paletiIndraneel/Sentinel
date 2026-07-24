#include "InternetMonitor.h"
#include "config.h"
#include <cstring>


String getSignalStrength(int rssi)
{
    if (rssi >= -50) return "Excellent";
    if (rssi >= -60) return "Strong";
    if (rssi >= -70) return "Good";
    if (rssi >= -80) return "Fair";
    if (rssi >= -90) return "Weak";
    return "Critical";
}

namespace sentinel {

void InternetMonitor::begin() {
    WiFi.mode(WIFI_STA);
    client_.onConnect(onConnect, this);
    client_.onData(onData, this);
    client_.onDisconnect(onDisconnect, this);
    client_.onError(onError, this);
    lastWifiAttemptMs_ = millis() - config::WIFI_RETRY_INTERVAL_MS;
    lastProbeMs_       = millis() - config::INTERNET_TEST_INTERVAL_MS;
}

void InternetMonitor::update(uint32_t nowMs) {
    if (strlen(config::WIFI_SSID) == 0) {
        static bool reported = false;
        if (!reported) {
            Serial.println("[Wi-Fi] No SSID configured.");
            reported = true;
        }
        return;
    }

    if (WiFi.status() != WL_CONNECTED) {
        if (!connecting_ &&
            nowMs - lastWifiAttemptMs_ >= config::WIFI_RETRY_INTERVAL_MS) {
            // Serial.println("[Wi-Fi] Starting connection attempt.");
            WiFi.begin(config::WIFI_SSID, config::WIFI_PASSWORD);
            connecting_ = true;
            connectStartedMs_ = nowMs;
            lastWifiAttemptMs_ = nowMs;
        }
        if (connecting_ &&nowMs - connectStartedMs_ >= config::WIFI_CONNECT_TIMEOUT_MS) {
            Serial.println("[Wi-Fi] Connection timeout.");
            WiFi.disconnect();
            connecting_ = false;
        }
        if (probeRunning_) {
            client_.close();
            probeRunning_ = false;
        }
        if (hadResult_) {
            setInternetAvailable(false);
        }
        return;
    }

    if (connecting_) {
    connecting_ = false;
    printConnectionInfo();
}

    if (!probeRunning_ &&
        nowMs - lastProbeMs_ >= config::INTERNET_TEST_INTERVAL_MS) {
        beginProbe(nowMs);
    }

    if (probeRunning_ &&nowMs - probeStartedMs_ >= config::INTERNET_TEST_TIMEOUT_MS) {
        Serial.println("[Internet] Probe timeout.");
        client_.close();
        probeRunning_ = false;
        lastProbeMs_ = nowMs;
        setInternetAvailable(false);
    }
}

void InternetMonitor::beginProbe(uint32_t nowMs) {
    // Serial.printf("[Internet] Connecting to %s:%u\n",config::INTERNET_TEST_HOST,config::INTERNET_TEST_PORT);
    probeRunning_ =client_.connect(config::INTERNET_TEST_HOST,config::INTERNET_TEST_PORT);
    probeStartedMs_ = nowMs;
    lastProbeMs_ = nowMs;
    if (!probeRunning_) {
        Serial.println("[Internet] TCP connect failed.");
        setInternetAvailable(false);
    }
}

void InternetMonitor::onConnect(void* arg, AsyncClient* client) {
    auto* self = static_cast<InternetMonitor*>(arg);
    // Serial.println("[Internet] TCP connected.");
    String request =
        "HEAD / HTTP/1.1\r\n"
        "Host: " + String(config::INTERNET_TEST_HOST) + "\r\n"
        "Connection: close\r\n\r\n";
    client->write(request.c_str());
    self->probeRunning_ = true;
}

void InternetMonitor::onData(void* arg,AsyncClient* client,void* data,size_t len) {
    auto* self = static_cast<InternetMonitor*>(arg);
    String response((char*)data, len);
    // Parse Date header only
    int start = response.indexOf("Date:");
    if (start >= 0) {
        int end = response.indexOf("\r\n", start);
        if (end > start) {
            String date = response.substring(start + 5, end);
            date.trim();
            if (self->dateCallback_) {
                self->dateCallback_(date);
            }
        }
    }
    bool success =
        response.indexOf("HTTP/1.1 200") >= 0 ||
        response.indexOf("HTTP/1.0 200") >= 0 ||
        response.indexOf("HTTP/1.1 301") >= 0 ||
        response.indexOf("HTTP/1.0 301") >= 0 ||
        response.indexOf("HTTP/1.1 302") >= 0 ||
        response.indexOf("HTTP/1.0 302") >= 0;
    self->setInternetAvailable(success);
    self->probeRunning_ = false;
    client->close();
}

void InternetMonitor::onDisconnect(void* arg, AsyncClient*) {
    auto* self = static_cast<InternetMonitor*>(arg);
    if (self->probeRunning_) {
        Serial.println("[Internet] Connection closed before response.");
        self->probeRunning_ = false;
        self->setInternetAvailable(false);
    }
}

void InternetMonitor::onError(void* arg,AsyncClient*,int8_t error) {
    auto* self = static_cast<InternetMonitor*>(arg);
    Serial.printf("[Internet] TCP Error: %d\n", error);
    self->probeRunning_ = false;
    self->setInternetAvailable(false);
}

void InternetMonitor::setInternetAvailable(bool value) {
    if (!hadResult_ || internetAvailable_ != value) {
        internetAvailable_ = value;
        changed_ = true;
    }
    hadResult_ = true;
}

bool InternetMonitor::takeStatusChanged() {
    bool changed = changed_;
    changed_ = false;
    return changed;
}

void InternetMonitor::setDateCallback(DateCallback cb) {
    dateCallback_ = cb;
}
bool InternetMonitor::isInternetAvailable() const {
    return internetAvailable_;
}
void InternetMonitor::printConnectionInfo() const
{
    Serial.println();
    Serial.println("========== Wi-Fi Connected ==========");
    Serial.printf("SSID    : %s\n", WiFi.SSID().c_str());
    Serial.printf("IP      : %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Gateway : %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("DNS     : %s\n", WiFi.dnsIP().toString().c_str());
    Serial.printf("MAC     : %s\n", WiFi.macAddress().c_str());
    Serial.printf("RSSI    : %d dBm\n", WiFi.RSSI());
    Serial.printf("Channel : %d\n", WiFi.channel());
    Serial.println("====================================");
    Serial.println();
}
String InternetMonitor::getConnectionInfo() const
{
    String msg;

    msg += "📶 Wi-Fi \n\n";
    msg += "SSID     : " + WiFi.SSID() + "\n";
    msg += "IP       : " + WiFi.localIP().toString() + "\n";
    msg += "Gateway  : " + WiFi.gatewayIP().toString() + "\n";
    msg += "DNS      : " + WiFi.dnsIP().toString() + "\n";
    msg += "RSSI     : " + String(WiFi.RSSI()) + " dBm\n";
    msg += "MAC      : " + WiFi.macAddress() + "\n";
    msg += "Signal   : " + getSignalStrength(WiFi.RSSI()) + "\n";
    msg += "Channel  : " + String(WiFi.channel());

    return msg;
}
} // namespace sentinel